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
    <shader>
        <![CDATA[
            varying vec2 texcoord;

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

            const vec3 lumfact = vec3(0.2125, 0.7154, 0.0721);

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

#if SHADER_API_VERTEX
            void PostProcessVS()
            {
                texcoord = glsl_Texcoord;
                gl_Position = glsl_Position;
            }
#endif

#if SHADER_API_FRAGMENT
            void SamplePS()
            {
                vec4 color = texture2D(texSource, texcoord.xy);
                color += textureOffset(texSource, texcoord, ivec2(0, -1));
                color += textureOffset(texSource, texcoord, ivec2(1,  0));
                color += textureOffset(texSource, texcoord, ivec2(1, -1));
                color /= 4.0;

                glsl_FragColor0 = color;
            }

            void SampleLogPS()
            {
                vec4 color = texture2D(texSource, texcoord.xy);
                color += textureOffset(texSource, texcoord, ivec2(0, -1));
                color += textureOffset(texSource, texcoord, ivec2(1,  0));
                color += textureOffset(texSource, texcoord, ivec2(1, -1));
                color /= 4.0;

                glsl_FragColor0.r = log(dot(color.rgb, lumfact) + 0.00001);
            }

            void BloomPS()
            {
                vec3 color = texture2D(texSource, texcoord).rgb;

                float lum = dot(color, lumfact);

                vec3 L = color * (lum / lumAve);
                L = L / (L + vec3(1));
                float luml = (L.x + L.y + L.z) / 3;

                glsl_FragColor0.rgb = L * mix(1.0, 0.0, pow(max(0.8 - luml, 0), 0.2));
            }

            void BlurhPS()
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

            void BlurvPS()
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

            void TonePS()
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

                glsl_FragColor0 = vec4(fimic, lum);
            }
#endif
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="sample">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="SamplePS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="samplelog">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="SampleLogPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="bloom">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="BloomPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurh">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="BlurhPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurv">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="BlurvPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="tone">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="TonePS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true" />
            <state name="blendSeparate" value="true" />
            <state name="blendsrc" value="one" />
            <state name="blenddst" value="zero" />
            <state name="blendalphasrc" value="one" />
            <state name="blendalphadst" value="zero" />
        </pass>
    </technique>
</effect>