<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <parameter name="decal" type="sampler2D" />
    <parameter name="bloom" type="sampler2D" />
    <parameter name="avelum" type="float" />
    <parameter name="key" type="float" />
    <shader type="vertex" name="mainVS">
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

            uniform sampler2D decal;

            in vec2 texcoord;

            void main()
            {
                vec4 color = texture2D(decal, texcoord.xy);
                color += textureOffset(decal, texcoord, ivec2(0, -1));
                color += textureOffset(decal, texcoord, ivec2(1,  0));
                color += textureOffset(decal, texcoord, ivec2(1, -1));
                color /= 4.0;

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <shader type="fragment" name="samplelog">
        <![CDATA[
            #version 330 core

            uniform sampler2D decal;

            layout(location = 0) out float glsl_FragColor0;

            in vec2 texcoord;

            const vec3 lumfact = vec3(0.2125, 0.7154, 0.0721);

            void main()
            {
                vec4 color = texture2D(decal, texcoord.xy);
                color += textureOffset(decal, texcoord, ivec2(0, -1));
                color += textureOffset(decal, texcoord, ivec2(1,  0));
                color += textureOffset(decal, texcoord, ivec2(1, -1));
                color /= 4.0;

                glsl_FragColor0 = log(dot(color.rgb, lumfact) + 0.00001);
            }
        ]]>
    </shader>
    <shader type="fragment" name="bloom">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D decal;
            uniform float avelum;

            in vec2 texcoord;

            const vec3 lumfact = vec3(0.2125, 0.7154, 0.0721);

            void main()
            {
               vec4 color = texture2D(decal, texcoord);

                float lum = dot(color.rgb, lumfact);

                vec4 L = color * (lum / avelum);
                L = L / (L + vec4(1));

                float luml = (L.x + L.y + L.z) / 3;
                if (luml < 0.8)
                    discard;

                glsl_FragColor0 = L;
            }
        ]]>
    </shader>
    <shader type="fragment" name="blurh">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D decal;

            in vec2 texcoord;

            void main()
            {
                vec4 color = vec4(0);
                color += texture2D(decal, texcoord);
                color += textureOffset(decal, texcoord, ivec2(-5, 0)) * 0.1;
                color += textureOffset(decal, texcoord, ivec2(-4, 0)) * 0.22;
                color += textureOffset(decal, texcoord, ivec2(-3, 0)) * 0.358;
                color += textureOffset(decal, texcoord, ivec2(-2, 0)) * 0.523;
                color += textureOffset(decal, texcoord, ivec2(-1, 0)) * 0.843;
                color += textureOffset(decal, texcoord, ivec2( 1, 0)) * 0.843;
                color += textureOffset(decal, texcoord, ivec2( 2, 0)) * 0.523;
                color += textureOffset(decal, texcoord, ivec2( 3, 0)) * 0.358;
                color += textureOffset(decal, texcoord, ivec2( 4, 0)) * 0.22;
                color += textureOffset(decal, texcoord, ivec2( 5, 0)) * 0.1;
                color /= 5.0;

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <shader type="fragment" name="blurv">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D decal;

            in vec2 texcoord;

            void main()
            {
                vec4 color = vec4(0);
                color += texture2D(decal, texcoord);
                color += textureOffset(decal, texcoord, ivec2(0, -5)) * 0.1;
                color += textureOffset(decal, texcoord, ivec2(0, -4)) * 0.22;
                color += textureOffset(decal, texcoord, ivec2(0, -3)) * 0.358;
                color += textureOffset(decal, texcoord, ivec2(0, -2)) * 0.523;
                color += textureOffset(decal, texcoord, ivec2(0, -1)) * 0.843;
                color += textureOffset(decal, texcoord, ivec2(0,  1)) * 0.843;
                color += textureOffset(decal, texcoord, ivec2(0,  2)) * 0.523;
                color += textureOffset(decal, texcoord, ivec2(0,  3)) * 0.358;
                color += textureOffset(decal, texcoord, ivec2(0,  4)) * 0.22;
                color += textureOffset(decal, texcoord, ivec2(0,  5)) * 0.1;
                color /= 5.0;

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <shader type="fragment" name="tone">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

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

            vec4 filmicToneMapping(
                vec2 coord,
                sampler2D source,
                sampler2D bloom,
                vec3 gray,
                float lumAve,
                float lumKey,
                float exposure,
                float exposureBias,
                float linearRef,
                float linearContrast,
                vec3 linearSaturation,
                float innerVignette,
                float outerVignette,
                float gamma)
            {
                vec3 linearColor = texture2D(source, coord).rgb;
                vec3 linearGrey = vec3(dot(linearColor, gray));

                linearColor = linearColor * (lumKey / lumAve);
                linearColor = linearColor / (linearColor + vec3(1.0));
                linearColor = linearColor + texture2D(bloom, coord).rgb;
                linearColor = linearColor * exposure;
                linearColor = linearRef + linearContrast * (linearColor - linearRef);
                linearColor = linearGrey +  linearSaturation * (linearColor - linearGrey);
                linearColor = clamp(linearColor, 0, 1);

                vec3 curr = exposureBias * uncharted2Tonemap(linearColor);
                vec3 whiteScale = uncharted2Tonemap(vec3(W));
                vec3 fimic = curr / whiteScale;

                float L = length(coord * 2.0 - 1.0);
                float vignette = (1.0 - smoothstep(innerVignette, outerVignette, L));

                fimic *= vignette;
                fimic = pow(fimic, vec3(gamma));

                return vec4(fimic, linearGrey.r);
            }

            in vec2 texcoord;

            uniform sampler2D decal;
            uniform sampler2D bloom;
            uniform float avelum;
            uniform float key;

            const float gamma = 2.2;
            const float exposure = 1.0;
            const float exposureBias = 3.3;
            const float linearRef = 0.0;
            const float linearContrast = 1.0;
            const vec3 linearSaturation = vec3(1.0, 1.0, 1.0);
            const vec3 lumfact = vec3(0.2125, 0.7154, 0.0721);
            const float innerVignette = 0.7;
            const float outerVignette = 2.5;

            void main()
            {
                glsl_FragColor0 = filmicToneMapping(
                    texcoord,
                    decal,
                    bloom,
                    lumfact,
                    avelum,
                    key,
                    exposure,
                    exposureBias,
                    linearRef,
                    linearContrast,
                    linearSaturation,
                    innerVignette,
                    outerVignette,
                    gamma
                );
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="sample">
            <state name="vertex" value="mainVS" />
            <state name="fragment" value="sample" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="samplelog">
            <state name="vertex" value="mainVS" />
            <state name="fragment" value="samplelog" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="bloom">
            <state name="vertex" value="mainVS" />
            <state name="fragment" value="bloom" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurh">
            <state name="vertex" value="mainVS" />
            <state name="fragment" value="blurh" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurv">
            <state name="vertex" value="mainVS" />
            <state name="fragment" value="blurv" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="tone">
            <state name="vertex" value="mainVS" />
            <state name="fragment" value="tone" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
    </technique>
</effect>