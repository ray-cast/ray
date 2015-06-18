<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <parameter name="texSizeInv" type="float2" />
    <parameter name="texSource" type="sampler2D" />
    <parameter name="texBloom" type="sampler2D" />
    <parameter name="vignetteEnable" type="bool" />
    <parameter name="vignetteInner" type="float" />
    <parameter name="vignetteOuter" type="float" />
    <parameter name="bloomFactor" type="float" />
    <parameter name="bloomIntensity" type="float" />
    <parameter name="exposure" type="float" />
    <parameter name="exposureBias" type="float" />
    <parameter name="lumAve" type="float" />
    <parameter name="lumKey" type="float" />
    <parameter name="lumMax" type="float" />
    <shader>
        <![CDATA[
            #extension GL_ARB_bindless_texture : require

            uniform sampler2D texSource;
            uniform sampler2D texBloom;

            uniform float bloomFactor;
            uniform float bloomIntensity;

            uniform float exposure;
            uniform float exposureBias;

            uniform bool vignetteEnable;
            uniform float vignetteInner;
            uniform float vignetteOuter;

            uniform float lumAve;
            uniform float lumKey;
            uniform float lumMax;

            uniform vec2 texSizeInv;

            varying vec2 coord;
            varying vec2 blurCoordinates[11];

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
                coord = glsl_Texcoord;
                gl_Position = glsl_Position;
            }

            void BlurvVS()
            {
                blurCoordinates[0] = glsl_Texcoord;
                blurCoordinates[1] = glsl_Texcoord + vec2(-5.0 * texSizeInv.x, 0);
                blurCoordinates[2] = glsl_Texcoord + vec2(-4.0 * texSizeInv.x, 0);
                blurCoordinates[3] = glsl_Texcoord + vec2(-3.0 * texSizeInv.x, 0);
                blurCoordinates[4] = glsl_Texcoord + vec2(-2.0 * texSizeInv.x, 0);
                blurCoordinates[5] = glsl_Texcoord + vec2(-1.0 * texSizeInv.x, 0);
                blurCoordinates[6] = glsl_Texcoord + vec2( 1.0 * texSizeInv.x, 0);
                blurCoordinates[7] = glsl_Texcoord + vec2( 2.0 * texSizeInv.x, 0);
                blurCoordinates[8] = glsl_Texcoord + vec2( 3.0 * texSizeInv.x, 0);
                blurCoordinates[9] = glsl_Texcoord + vec2( 4.0 * texSizeInv.x, 0);
                blurCoordinates[10] = glsl_Texcoord + vec2( 5.0 * texSizeInv.x, 0);

                gl_Position = glsl_Position;
            }

            void BlurhVS()
            {
                blurCoordinates[0] = glsl_Texcoord;
                blurCoordinates[1] = glsl_Texcoord + vec2(0, -5.0 * texSizeInv.x);
                blurCoordinates[2] = glsl_Texcoord + vec2(0, -4.0 * texSizeInv.x);
                blurCoordinates[3] = glsl_Texcoord + vec2(0, -3.0 * texSizeInv.x);
                blurCoordinates[4] = glsl_Texcoord + vec2(0, -2.0 * texSizeInv.x);
                blurCoordinates[5] = glsl_Texcoord + vec2(0, -1.0 * texSizeInv.x);
                blurCoordinates[6] = glsl_Texcoord + vec2(0,  1.0 * texSizeInv.x);
                blurCoordinates[7] = glsl_Texcoord + vec2(0,  2.0 * texSizeInv.x);
                blurCoordinates[8] = glsl_Texcoord + vec2(0,  3.0 * texSizeInv.x);
                blurCoordinates[9] = glsl_Texcoord + vec2(0,  4.0 * texSizeInv.x);
                blurCoordinates[10] = glsl_Texcoord + vec2(0,  5.0 * texSizeInv.x);

                gl_Position = glsl_Position;
            }
#endif

#if SHADER_API_FRAGMENT
            void SamplePS()
            {
                vec2 inv = vec2(1) / textureSize(texSource, 0);

                vec4 color = texture2D(texSource, coord.xy);
                color += texture2D(texSource, coord + vec2(0.0, -1.0) * inv);
                color += texture2D(texSource, coord + vec2(1.0,  0.0) * inv);
                color += texture2D(texSource, coord + vec2(1.0, -1.0) * inv);
                color *= 0.25;

                glsl_FragColor0 = color;
            }

            void SampleLogPS()
            {
                vec2 inv = vec2(1) / textureSize(texSource, 0);

                vec4 color = texture2D(texSource, coord.xy);
                color += texture2D(texSource, coord + vec2(0.0, -1.0) * inv);
                color += texture2D(texSource, coord + vec2(1.0,  0.0) * inv);
                color += texture2D(texSource, coord + vec2(1.0, -1.0) * inv);
                color *= 0.25;

                const float epsilon = 0.0001;
                glsl_FragColor0.r = log(dot(color.rgb, lumfact) + epsilon);
            }

            void BloomPS()
            {
                vec3 color = texture2D(texSource, coord).rgb;
                float lum = dot(color, lumfact);
                glsl_FragColor0.rgb = color * max(lum - bloomFactor, 0) * bloomIntensity;
            }

            void BlurPS()
            {
                vec4 color = vec4(0);
                color += texture2D(texSource, blurCoordinates[0]) * 0.2;
                color += texture2D(texSource, blurCoordinates[1]) * 0.02;
                color += texture2D(texSource, blurCoordinates[2]) * 0.044;
                color += texture2D(texSource, blurCoordinates[3]) * 0.0716;
                color += texture2D(texSource, blurCoordinates[4]) * 0.1046;
                color += texture2D(texSource, blurCoordinates[5]) * 0.1686;
                color += texture2D(texSource, blurCoordinates[6]) * 0.1686;
                color += texture2D(texSource, blurCoordinates[7]) * 0.1046;
                color += texture2D(texSource, blurCoordinates[8]) * 0.0716;
                color += texture2D(texSource, blurCoordinates[9]) * 0.044;
                color += texture2D(texSource, blurCoordinates[10]) * 0.02;

                glsl_FragColor0 = color;
            }

            void TonePS()
            {
                vec3 color = texture2D(texSource, coord).rgb;
                vec3 bloom =  texture2D(texBloom, coord).rgb;

                float lum = max(dot(lumfact, color), 1e-10);
                float lumScale = lumKey / lumAve;

                vec3 scaleColor = color * lumScale;
                scaleColor = scaleColor + bloom;
                scaleColor = scaleColor * exposure;

                vec3 curr = exposureBias * uncharted2Tonemap(scaleColor);
                vec3 whiteScale = uncharted2Tonemap(vec3(W));
                vec3 fimic = curr / whiteScale;

                if (vignetteEnable)
                {
                    float L = length(coord * 2.0 - 1.0);
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
            <state name="vertex" value="BlurhVS" />
            <state name="fragment" value="BlurPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurv">
            <state name="vertex" value="BlurvVS" />
            <state name="fragment" value="BlurPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="tone">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="TonePS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
    </technique>
</effect>