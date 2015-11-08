<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="texSource" type="sampler2D" />
    <parameter name="texBloom" type="sampler2D" />
    <parameter name="bloomThreshold" type="float" />
    <parameter name="bloomIntensity" type="float" />
    <parameter name="bloomSizeInv" type="float2" />
    <parameter name="exposure" type="float" />
    <parameter name="burnout" type="float"/>
    <parameter name="lumAve" type="float" />
    <parameter name="lumKey" type="float" />
    <shader name="vertex">
        <![CDATA[
            out float2 coord;

            void PostProcessVS()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            #define TONEMAP_LINEAR 0
            #define TONEMAP_EXPONENTIAL 1
            #define TONEMAP_EXPONENTIAL_HSV 2
            #define TONEMAP_REINHARD 3
            #define TONEMAP_FILMIC 4

            #ifndef TONEMAP_OPERATOR
            #define TONEMAP_OPERATOR TONEMAP_FILMIC
            #endif

            uniform sampler2D texSource;
            uniform sampler2D texBloom;

            uniform float bloomThreshold;
            uniform float bloomIntensity;
            uniform float2 bloomSizeInv;

            uniform float lumAve;
            uniform float lumKey;

            uniform float exposure;
            uniform float burnout;

            in float2 coord;

            const float A = 0.15; // Shoulder Strength
            const float B = 0.50; // Linear Strength
            const float C = 0.10; // Linear Angle
            const float D = 0.20; // Toe Strength
            const float E = 0.02; // Toe Numerator
            const float F = 0.30; // Toe Denominator E/F = Toe Angle
            const float W = 11.2; // Linear White Point Value

            float3 Uncharted2Tonemap(float3 x)
            {
                return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
            }

            float3 FilmicTonemap(float3 color, float exposure)
            {
                #if TONEMAP_OPERATOR == TONEMAP_LINEAR
                    return exposure * color;
                #elif TONEMAP_OPERATOR == TONEMAP_EXPONENTIAL
                    color = 1.0 - exp2(-exposure * color);
                    return color;
                #elif TONEMAP_OPERATOR == TONEMAP_EXPONENTIAL_HSV
                    color = rgb2hsv(color);
                    color.b = 1.0 - exp2(-exposure * color.b);
                    color = hsv2rgb(color);
                    return color;
                #elif TONEMAP_OPERATOR == TONEMAP_REINHARD
                    color = xyz2Yxy(rgb2xyz(color));
                    float L = color.r;
                    L *= exposure;
                    float LL = 1 + L / (burnout * burnout);
                    float L_d = L * LL / (1 + L);
                    color.r = L_d;
                    color = xyz2rgb(Yxy2xyz(color));
                    return color;
                #else // TONEMAP_FILMIC
                    color = 2.2f * Uncharted2Tonemap(exposure * color);
                    float3 whiteScale = 1.0f / Uncharted2Tonemap(float3(W));
                    color *= whiteScale;
                    return color;
                #endif
            }

            void SamplePS()
            {
                float4 color = sampleCoord(texSource, coord.xy);
                color += sampleCoordOffset(texSource, coord, int2(0, -1));
                color += sampleCoordOffset(texSource, coord, int2(1,  0));
                color += sampleCoordOffset(texSource, coord, int2(1, -1));
                color *= 0.25;

                glsl_FragColor0 = color;
            }

            void SampleLogPS()
            {
                float lum = sampleCoord(texSource, coord.xy).r;
                lum += sampleCoordOffset(texSource, coord, int2(0, -1)).r;
                lum += sampleCoordOffset(texSource, coord, int2(1,  0)).r;
                lum += sampleCoordOffset(texSource, coord, int2(1, -1)).r;
                lum *= 0.25;

                const float epsilon = 0.0001;
                glsl_FragColor0 = float4(log(lum + epsilon));
            }

            void BloomPS()
            {
                float4 color = sampleCoord(texSource, coord);
                glsl_FragColor0 = color * max(luminance(color.rgb) - bloomThreshold, 0.0) * bloomIntensity;
            }

            void BlurhPS()
            {
                float4 color;
                color  = sampleCoord(texSource, coord) * 0.2;
                color += sampleCoordOffset(texSource, coord, int2(-5, 0)) * 0.02;
                color += sampleCoordOffset(texSource, coord, int2(-4, 0)) * 0.044;
                color += sampleCoordOffset(texSource, coord, int2(-3, 0)) * 0.0716;
                color += sampleCoordOffset(texSource, coord, int2(-2, 0)) * 0.1046;
                color += sampleCoordOffset(texSource, coord, int2(-1, 0)) * 0.1686;
                color += sampleCoordOffset(texSource, coord, int2( 1, 0)) * 0.1686;
                color += sampleCoordOffset(texSource, coord, int2( 2, 0)) * 0.1046;
                color += sampleCoordOffset(texSource, coord, int2( 3, 0)) * 0.0716;
                color += sampleCoordOffset(texSource, coord, int2( 4, 0)) * 0.044;
                color += sampleCoordOffset(texSource, coord, int2( 5, 0)) * 0.02;

                glsl_FragColor0 = float4(color);
            }

           void BlurvPS()
            {
                float4 color;
                color  = sampleCoord(texSource, coord) * 0.2;
                color += sampleCoordOffset(texSource, coord, int2(0, -5)) * 0.02;
                color += sampleCoordOffset(texSource, coord, int2(0, -4)) * 0.044;
                color += sampleCoordOffset(texSource, coord, int2(0, -3)) * 0.0716;
                color += sampleCoordOffset(texSource, coord, int2(0, -2)) * 0.1046;
                color += sampleCoordOffset(texSource, coord, int2(0, -1)) * 0.1686;
                color += sampleCoordOffset(texSource, coord, int2(0,  1)) * 0.1686;
                color += sampleCoordOffset(texSource, coord, int2(0,  2)) * 0.1046;
                color += sampleCoordOffset(texSource, coord, int2(0,  3)) * 0.0716;
                color += sampleCoordOffset(texSource, coord, int2(0,  4)) * 0.044;
                color += sampleCoordOffset(texSource, coord, int2(0,  5)) * 0.02;

                glsl_FragColor0 = float4(color);
            }

            float CaculateVignette(float2 P, float inner, float outer)
            {
                float L = length(P);
                return (1.0 - smoothstep(inner, outer, L) );
            }

            void FimicTongMappingPS()
            {
                float3 color = sampleCoord(texSource, coord).rgb;
                float3 bloom =  sampleCoord(texBloom, coord).rgb;

                color = color * (lumKey / lumAve) + bloom;
                color = FilmicTonemap(color, exposure);

                glsl_FragColor0 = float4(color, 1.0);
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="Sample">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="SamplePS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front"/>
        </pass>
        <pass name="SampleLog">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="SampleLogPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front"/>
        </pass>
        <pass name="GenerateBloom">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="BloomPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front"/>
        </pass>
        <pass name="BlurBloomh">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="BlurhPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front"/>
        </pass>
        <pass name="BlurBloomv">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="BlurvPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front"/>
        </pass>
        <pass name="FimicTongMapping">
            <state name="vertex" value="PostProcessVS" />
            <state name="fragment" value="FimicTongMappingPS" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front"/>
        </pass>
    </technique>
</effect>