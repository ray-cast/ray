<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <parameter name="texSource" type="sampler2D" />
    <parameter name="texBloom" type="sampler2D" />
    <parameter name="lumAve" type="float" />
    <parameter name="lumKey" type="float" />
    <parameter name="lumMax" type="float" />
    <parameter name="exposure" type="float" />
    <parameter name="exposureBias" type="float" />
    <parameter name="vignetteEnable" type="int" />
    <parameter name="vignetteInner" type="float" />
    <parameter name="vignetteOuter" type="float" />
    <parameter name="gamma" type="float" />
    <shader type="vertex" name="postprocess">
        <![CDATA[
            #version 330 core

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
    <shader type="fragment" name="sample">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texSource;

            in vec2 texcoord;

            void main()
            {
                vec4 color = texture2D(texSource, texcoord.xy);
                color += textureOffset(texSource, texcoord, ivec2(0, -1));
                color += textureOffset(texSource, texcoord, ivec2(1,  0));
                color += textureOffset(texSource, texcoord, ivec2(1, -1));
                color /= 4.0;

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <shader type="fragment" name="samplelog">
        <![CDATA[
            #version 330 core

            uniform sampler2D texSource;

            layout(location = 0) out float glsl_FragColor0;

            in vec2 texcoord;

            const vec3 lumfact = vec3(0.2125, 0.7154, 0.0721);

            void main()
            {
                vec4 color = texture2D(texSource, texcoord.xy);
                color += textureOffset(texSource, texcoord, ivec2(0, -1));
                color += textureOffset(texSource, texcoord, ivec2(1,  0));
                color += textureOffset(texSource, texcoord, ivec2(1, -1));
                color /= 4.0;

                glsl_FragColor0 = log(dot(color.rgb, lumfact) + 0.00001);
            }
        ]]>
    </shader>
    <shader type="fragment" name="bloom">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec3 glsl_FragColor0;

            uniform sampler2D texSource;
            uniform float lumAve;

            in vec2 texcoord;

            const vec3 lumfact = vec3(0.2125, 0.7154, 0.0721);

            void main()
            {
                vec3 color = texture2D(texSource, texcoord).rgb;

                float lum = dot(color, lumfact);

                vec3 L = color * (lum / lumAve);
                L = L / (L + vec3(1));
                float luml = (L.x + L.y + L.z) / 3;

                glsl_FragColor0 = L * mix(1.0, 0.0, pow(max(0.8 - luml, 0), 0.2));
            }
        ]]>
    </shader>
    <shader type="fragment" name="blurh">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texSource;

            in vec2 texcoord;

            void main()
            {
                vec4 color = vec4(0);
                color += texture2D(texSource, texcoord);
                color += textureOffset(texSource, texcoord, ivec2(-5, 0)) * 0.1;
                color += textureOffset(texSource, texcoord, ivec2(-4, 0)) * 0.22;
                color += textureOffset(texSource, texcoord, ivec2(-3, 0)) * 0.358;
                color += textureOffset(texSource, texcoord, ivec2(-2, 0)) * 0.523;
                color += textureOffset(texSource, texcoord, ivec2(-1, 0)) * 0.843;
                color += textureOffset(texSource, texcoord, ivec2( 1, 0)) * 0.843;
                color += textureOffset(texSource, texcoord, ivec2( 2, 0)) * 0.523;
                color += textureOffset(texSource, texcoord, ivec2( 3, 0)) * 0.358;
                color += textureOffset(texSource, texcoord, ivec2( 4, 0)) * 0.22;
                color += textureOffset(texSource, texcoord, ivec2( 5, 0)) * 0.1;
                color /= 5.0;

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <shader type="fragment" name="blurv">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texSource;

            in vec2 texcoord;

            void main()
            {
                vec4 color = vec4(0);
                color += texture2D(texSource, texcoord);
                color += textureOffset(texSource, texcoord, ivec2(0, -5)) * 0.1;
                color += textureOffset(texSource, texcoord, ivec2(0, -4)) * 0.22;
                color += textureOffset(texSource, texcoord, ivec2(0, -3)) * 0.358;
                color += textureOffset(texSource, texcoord, ivec2(0, -2)) * 0.523;
                color += textureOffset(texSource, texcoord, ivec2(0, -1)) * 0.843;
                color += textureOffset(texSource, texcoord, ivec2(0,  1)) * 0.843;
                color += textureOffset(texSource, texcoord, ivec2(0,  2)) * 0.523;
                color += textureOffset(texSource, texcoord, ivec2(0,  3)) * 0.358;
                color += textureOffset(texSource, texcoord, ivec2(0,  4)) * 0.22;
                color += textureOffset(texSource, texcoord, ivec2(0,  5)) * 0.1;
                color /= 5.0;

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <shader type="fragment" name="tone">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            in vec2 texcoord;

            uniform sampler2D texSource;
            uniform sampler2D texBloom;

            uniform float lumAve;
            uniform float lumKey;
            uniform float lumMax;

            uniform float exposure;
            uniform float exposureBias;

            uniform bool vignetteEnable;
            uniform float vignetteInner;
            uniform float vignetteOuter;

            uniform float gamma;

            const float A = 0.20; // Shoulder Strength
            const float B = 0.30; // Linear Strength
            const float C = 0.10; // Linear Angle
            const float D = 0.20; // Toe Strength
            const float E = 0.01; // Toe Numerator
            const float F = 0.30; // Toe Denominator E/F = Toe Angle
            const float W = 11.2; // Linear White Point Value

            vec3 uncharted2Tonemap(vec3 x)
            {
                return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
            }

            const vec3 lumfact = vec3(0.2125, 0.7154, 0.0721);

            void main()
            {
                vec3 color = texture2D(texSource, texcoord).rgb;
                vec3 tone =  texture2D(texBloom, texcoord).rgb;

                float lum = dot(color, lumfact);

                color = color + tone;
                color = color * (lumKey * lum / lumAve);
                color = color / (color + vec3(1));
                color = color * exposure * (exposure / lumMax + 1.0) / (exposure + 1.0);

                vec3 curr = exposureBias * uncharted2Tonemap(color);
                vec3 whiteScale = uncharted2Tonemap(vec3(W));
                vec3 fimic = curr / whiteScale;

                if (vignetteEnable)
                {
                    float L = length(texcoord * 2.0 - 1.0);
                    float vignette = (1.0 - smoothstep(vignetteInner, vignetteOuter, L));
                    fimic *= vignette;
                }

                fimic = pow(fimic, vec3(gamma));

                glsl_FragColor0 = vec4(fimic, lum);
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="sample">
            <state name="vertex" value="postprocess" />
            <state name="fragment" value="sample" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="samplelog">
            <state name="vertex" value="postprocess" />
            <state name="fragment" value="samplelog" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="bloom">
            <state name="vertex" value="postprocess" />
            <state name="fragment" value="bloom" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurh">
            <state name="vertex" value="postprocess" />
            <state name="fragment" value="blurh" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurv">
            <state name="vertex" value="postprocess" />
            <state name="fragment" value="blurv" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="tone">
            <state name="vertex" value="postprocess" />
            <state name="fragment" value="tone" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
    </technique>
</effect>