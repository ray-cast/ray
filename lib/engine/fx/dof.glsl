<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="texColor" type="sampler2D" />
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texShrunk" type="sampler2D" />
    <parameter name="texBlured" type="sampler2D" />
    <parameter name="texSmall" type="sampler2D" />
    <parameter name="texLarge" type="sampler2D" />
    <parameter name="far" semantic="CameraFar"/>
    <parameter name="near" semantic="CameraNear"/>
    <shader type="vertex" name="DepthOfFieldVS">
        <![CDATA[
            layout(location = 0) in vec4 glsl_Vertex;
            layout(location = 2) in vec2 glsl_Texcoord;

            out vec2 texcoord;

            void main()
            {
                texcoord = glsl_Texcoord;
                gl_Position = glsl_Vertex;
            }
        ]]>
    </shader>
    <shader type="fragment" name="SamplePS">
        <![CDATA[
            uniform sampler2D texColor;
            uniform sampler2D texDepth;

            in vec2 texcoord;

            void main()
            {
                vec3 color = texture2D(texColor, texcoord).rgb;
                color += textureOffset(texColor, texcoord, ivec2(0, -1)).rgb;
                color += textureOffset(texColor, texcoord, ivec2(1,  0)).rgb;
                color += textureOffset(texColor, texcoord, ivec2(1, -1)).rgb;
                color /= 4.0;

                float depth = texture2D(texDepth, texcoord).r;
                depth += textureOffset(texDepth, texcoord, ivec2(0, -1)).r;
                depth += textureOffset(texDepth, texcoord, ivec2(1,  0)).r;
                depth += textureOffset(texDepth, texcoord, ivec2(1, -1)).r;
                depth /= 4.0;

                glsl_FragColor0 = vec4(color, depth);
            }
        ]]>
    </shader>
    <shader type="fragment" name="blurh">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texColor;

            in vec2 texcoord;

            void main()
            {
                vec4 color = vec4(0);
                color += texture2D(texColor, texcoord);
                color += textureOffset(texColor, texcoord, ivec2(-5, 0)) * 0.1;
                color += textureOffset(texColor, texcoord, ivec2(-4, 0)) * 0.22;
                color += textureOffset(texColor, texcoord, ivec2(-3, 0)) * 0.358;
                color += textureOffset(texColor, texcoord, ivec2(-2, 0)) * 0.523;
                color += textureOffset(texColor, texcoord, ivec2(-1, 0)) * 0.843;
                color += textureOffset(texColor, texcoord, ivec2( 1, 0)) * 0.843;
                color += textureOffset(texColor, texcoord, ivec2( 2, 0)) * 0.523;
                color += textureOffset(texColor, texcoord, ivec2( 3, 0)) * 0.358;
                color += textureOffset(texColor, texcoord, ivec2( 4, 0)) * 0.22;
                color += textureOffset(texColor, texcoord, ivec2( 5, 0)) * 0.1;
                color /= 5.0;

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <shader type="fragment" name="blurv">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texColor;

            in vec2 texcoord;

            void main()
            {
                vec4 color = vec4(0);
                color += texture2D(texColor, texcoord);
                color += textureOffset(texColor, texcoord, ivec2(0, -5)) * 0.1;
                color += textureOffset(texColor, texcoord, ivec2(0, -4)) * 0.22;
                color += textureOffset(texColor, texcoord, ivec2(0, -3)) * 0.358;
                color += textureOffset(texColor, texcoord, ivec2(0, -2)) * 0.523;
                color += textureOffset(texColor, texcoord, ivec2(0, -1)) * 0.843;
                color += textureOffset(texColor, texcoord, ivec2(0,  1)) * 0.843;
                color += textureOffset(texColor, texcoord, ivec2(0,  2)) * 0.523;
                color += textureOffset(texColor, texcoord, ivec2(0,  3)) * 0.358;
                color += textureOffset(texColor, texcoord, ivec2(0,  4)) * 0.22;
                color += textureOffset(texColor, texcoord, ivec2(0,  5)) * 0.1;
                color /= 5.0;

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <shader type="fragment" name="ComputeNear">
        <![CDATA[
            #version 330
            precision mediump float;

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texShrunk;
            uniform sampler2D texBlured;

            in vec2 texcoord;

            void main()
            {
                vec4 shrunk = texture2D(texShrunk, texcoord);
                vec4 blurred = texture2D(texBlured, texcoord);

                vec3 color = shrunk.rgb;
                float coc = 2 * max(blurred.a, shrunk.a) - shrunk.a;

                glsl_FragColor0 = vec4(color, coc);
            }
        ]]>
    </shader>
    <shader type="fragment" name="Final">
        <![CDATA[
            #version 330
            precision mediump float;

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texColor;
            uniform sampler2D texSmall;
            uniform sampler2D texLarge;
            uniform sampler2D texDepth;

            uniform float near;
            uniform float far;

            in vec2 texcoord;

            float d0 = 0.3;
            float d1 = 0.4;
            float d2 = 0.3;

            vec4 dofLerpScale = vec4(-1.0 / d0, -1 / d1, -1 / d2, 1 / d2);
            vec4 dofLerpBias = vec4(1, (1 - d2) / d1, 1 / d2, (d2 - 1) / d2);

            vec4 texOffset(sampler2D s, vec2 tc, vec2 offset)
            {
                vec2 size = textureSize(s, 0);
                return texture2D(s, tc + offset / size);
            }

            vec3 getSamllBlurSample(vec2 coord)
            {
                vec3 sum = vec3(0);
                float weight = 4.0 / 17.0;

                sum += weight * texOffset(texColor, texcoord, vec2( 0.5, -1.5)).rgb;
                sum += weight * texOffset(texColor, texcoord, vec2(-1.5, -0.5)).rgb;
                sum += weight * texOffset(texColor, texcoord, vec2(-0.5, -1.5)).rgb;
                sum += weight * texOffset(texColor, texcoord, vec2( 1.5,  0.5)).rgb;

                return sum;
            }

            vec4 interpolateDOF(vec3 small, vec3 med, vec3 large, float t)
            {
                vec4 weights = clamp(t * dofLerpScale + dofLerpBias, 0.0, 1.0);
                weights.yz = min(weights.yz, 1 - weights.xy);

                vec3 color = weights.y * small + weights.z * med + weights.w * large;
                float alpha = dot(weights.yzw, vec3(16.0 / 17, 1.0, 1.0));

                return vec4(color, alpha);
            }

            float linearizeDepth(float d)
            {
                return (2.0 * near * far) / ((far - near) * d - (far + near));
            }

            void main()
            {
                vec3 small = getSamllBlurSample(texcoord);
                vec4 med = texture2D(texSmall, texcoord);
                vec4 large = texture2D(texLarge, texcoord);

                float coc = 0.0;
                float near = med.a;
                float depth = texture2D(texDepth, texcoord).r * 2.0 - 1.0;

                if (depth > 1.0e6)
                {
                    coc = near;
                }
                else
                {
                    coc = max(near, depth);
                }

                glsl_FragColor0 = interpolateDOF(small, med.rgb, large.rgb, coc);
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="sample">
            <state name="vertex" value="DepthOfFieldVS" />
            <state name="fragment" value="SamplePS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurh">
            <state name="vertex" value="DepthOfFieldVS" />
            <state name="fragment" value="blurh" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurv">
            <state name="vertex" value="DepthOfFieldVS" />
            <state name="fragment" value="blurv" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="computeNear">
            <state name="vertex" value="DepthOfFieldVS" />
            <state name="fragment" value="ComputeNear" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="final">
            <state name="vertex" value="DepthOfFieldVS" />
            <state name="fragment" value="Final" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
    </technique>
</effect>