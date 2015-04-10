<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <parameter name="numSample" type="int"/>
    <parameter name="projScale" type="float"/>
    <parameter name="projInfo" type="float4"/>
    <parameter name="clipInfo" type="float4"/>
    <parameter name="radius" type="float"/>
    <parameter name="bias" type="float"/>
    <parameter name="epsilon" type="float"/>
    <parameter name="intensityDivR6" type="float"/>
    <parameter name="blurFactor" type="float" />
    <parameter name="blurSharpness" type="float" />
    <parameter name="blurRadius" type="int" />
    <parameter name="blurDirection" type="int2"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texNormal" semantic="NormalMap" />
    <parameter name="texAO" type="sampler2D" />
    <parameter name="texColor" type="sampler2D"/>
    <parameter name="texSource" type="sampler2D"/>
    <parameter name="matViewInverseTranspose" semantic="matViewInverseTranspose" />
    <shader type="vertex" name="mainVS">
        <![CDATA[
            #version 330 core

            layout(location = 0) in vec4 glsl_Position;
            layout(location = 2) in vec2 glsl_Texcoord;

            out vec4 position;
            out vec2 coord;

            void main()
            {
                coord = glsl_Texcoord;
                gl_Position = position = glsl_Position;
            }
        ]]>
    </shader>
    <shader type="fragment" name="aoPS">
        <![CDATA[
            #version 330

            layout(location = 0) out float glsl_FragColor0;

            #define NUM_SPIRAL_TURNS 7

            in vec4 position;
            in vec2 coord;

            uniform float radius;
            uniform float projScale;
            uniform float bias;
            uniform float epsilon;
            uniform float intensityDivR6;
            uniform vec4 projInfo;
            uniform vec4 clipInfo;

            uniform int numSample;

            uniform sampler2D texDepth;
            uniform sampler2D texNormal;

            uniform mat4 matViewInverseTranspose;

            float heaviside(float x)
            {
                return 0.5 * (1 + sign(x));
            }

            float linearizeDepth(vec2 uv)
            {
                float d = texture2D(texDepth, uv).r * 2.0 - 1.0;
                return clipInfo.x / (clipInfo.y * d - clipInfo.z);
            }

            vec3 getPosition(vec2 uv)
            {
                float d = linearizeDepth(uv);
                return vec3((projInfo.xy * uv + projInfo.zw) * d, d);
            }

            vec2 tapLocation(int sampleNumber, float spinAngle, float diskRadius)
            {
                float alpha = (sampleNumber + 0.5) * (1.0 / numSample);
                float angle = alpha * (NUM_SPIRAL_TURNS * 6.28) + spinAngle;
                return vec2(cos(angle), sin(angle)) * alpha * diskRadius;
            }

            float computeAO(vec3 c, vec3 n, vec3 q)
            {
                vec3 v = q - c;

                float vv = dot(v, v);
                float vn = dot(v, n);

                float f = max(radius * radius - vv, 0);
                return heaviside(f) * max((vn - bias) / (vv + epsilon), 0);
            }

            float sampleAO(int tapIndex, vec2 uv, vec3 c, vec3 n, float spinAngle, float diskRadius)
            {
                vec2 offset = tapLocation(tapIndex, spinAngle, diskRadius);

                vec3 q = getPosition(uv + offset);

                return computeAO(c, n, q);
            }

            void main()
            {
                ivec2 ssC = ivec2(gl_FragCoord.xy);

                vec3 c = getPosition(coord);
                vec3 n = mat3(matViewInverseTranspose) * texture2D(texNormal, coord).rgb;

                float spinAngle = (3 * ssC.x ^ ssC.y + ssC.x * ssC.y) * 10;
                float diskRadius = projScale * radius / c.z;

                float sum = 0.0;
                for (int i = 0; i < numSample; ++i)
                {
                    sum += sampleAO(i, coord, c, n, spinAngle, diskRadius);
                }

                float ao = max(0.0, 1.0 - sum / numSample * intensityDivR6);

                glsl_FragColor0 = ao;
            }
        ]]>
    </shader>
    <shader type="fragment" name="blurPS">
        <![CDATA[
            #version 330 core

            layout(location = 0) out float glsl_FragColor0;

            uniform sampler2D texSource;
            uniform sampler2D texDepth;

            uniform vec4 clipInfo;

            uniform int blurRadius;
            uniform float blurFactor;
            uniform float blurSharpness;
            uniform ivec2 blurDirection;

            float linearizeDepth(ivec2 uv)
            {
                float d = texelFetch(texDepth, uv, 0).r;
                return clipInfo.x / (clipInfo.y * d - clipInfo.z);
            }

            float bilateralfilter(ivec2 uv, float r, float center_c, float center_d)
            {
                float d = linearizeDepth(uv);
                float ddiff = d - center_d;
                return exp(-r * r * blurFactor - ddiff * ddiff * blurSharpness);
            }

            void main()
            {
                ivec2 ssC = ivec2(gl_FragCoord.xy);

                float center_c = texelFetch(texSource, ssC, 0).r;
                float center_d = linearizeDepth(ssC);

                float sum = 0;
                float total = 0;

                for (int r = -blurRadius; r <= blurRadius; r++)
                {
                    ivec2 offset = ssC + blurDirection * r;

                    float bilateralWeight = bilateralfilter(offset, r, center_c, center_d);

                    sum += texelFetch(texSource, offset, 0).r * bilateralWeight;
                    total += bilateralWeight;
                }

                const float epsilon = 0.0001;
                glsl_FragColor0 = sum / (total + epsilon);
            }
        ]]>
    </shader>
    <shader type="fragment" name="copyPS">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texAO;
            uniform sampler2D texColor;

            in vec2 coord;

            void main()
            {
                vec4 color = texture2D(texColor, coord);
                color.rgb *= texture2D(texAO, coord).r;
                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ao">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="aoPS"/>
            <state name="depthtest" value="false"/>
        </pass>
        <pass name="blur">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="blurPS"/>
        </pass>
        <pass name="copy">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="copyPS"/>
        </pass>
    </technique>
</effect>