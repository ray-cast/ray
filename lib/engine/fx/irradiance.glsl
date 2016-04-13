<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="paraboloidCubeMapSampler" type="sampler2D" />
    <parameter name="paraboloidSamplesInverse" type="float"/>
    <parameter name="SHConvolveDE0" type="sampler2D"/>
    <parameter name="SHConvolveDE1" type="sampler2D"/>
    <parameter name="SHConvolveYlmDW0" type="sampler2D"/>
    <parameter name="SHConvolveYlmDW1" type="sampler2D"/>
    <shader name="vertex">
        <![CDATA[
            out vec4 position;

            void ConvertHemisphereVS()
            {
                gl_Position = position = glsl_Position;
            }

            void ProjectDualParaboloidToSHVS()
            {
                gl_Position = position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            #extension GL_NV_shadow_samplers_cube : enable

            #define NUM_RADIANCE_SAMPLES  32
            #define NUM_RESULT_SAMPLES    32

            #define NUM_ORDER    3
            #define NUM_ORDER_P2 4

            in vec4 position;

            uniform sampler2D SHConvolveDE0;
            uniform sampler2D SHConvolveDE1;
            uniform sampler2D SHConvolveYlmDW0;
            uniform sampler2D SHConvolveYlmDW1;
            uniform sampler2D SH_Coefficients;

            uniform samplerCube paraboloidCubeMapSampler;

            uniform float paraboloidSamplesInverse;

            void ConvertHemispherePS()
            {
                float3 parabNormal;
                parabNormal.xy = ((position.xy + float2(0.5, 0.5)) * paraboloidSamplesInverse) * 2.f - 1.f;
                parabNormal.z = 1.0;
                parabNormal = normalize(parabNormal);

                glsl_FragColor0 = textureCube(paraboloidCubeMapSampler, reflect(float3(0.0, 0.0, 1.0), parabNormal));
                glsl_FragColor1 = textureCube(paraboloidCubeMapSampler, reflect(float3(0.0, 0.0, -1.0), parabNormal));
            }

            float2 ComputeWeightSampleLocation(float2 texel, float2 pixelpos)
            {
                float2 weightSample = (texel + pixelpos) / NUM_ORDER_P2;
                return weightSample;
            }

            float3 ConvolveSample(sampler2D weightMap, sampler2D colorMap, float2 coord, float2 pixelpos)
            {
                float3 color  = texture(colorMap,  coord).rgb;
                float  weight = texture(weightMap, ComputeWeightSampleLocation(coord, pixelpos)).r;
                return color * weight;
            }

            void ProjectDualParaboloidToSHPS()
            {
                float3 accum = float3(0, 0, 0);
                float2 coord = float2(0.5f / NUM_RADIANCE_SAMPLES, 0.5f / NUM_RADIANCE_SAMPLES);

                for (int j = 0; j < NUM_RADIANCE_SAMPLES; j++)
                {
                    coord.x =  0.5f / NUM_RADIANCE_SAMPLES;
                    coord.y += 1.0f / NUM_RADIANCE_SAMPLES;

                    for (int i = 0; i < NUM_RADIANCE_SAMPLES; i++)
                    {
                        coord.x += 1.0f / NUM_RADIANCE_SAMPLES;

                        accum += ConvolveSample(SHConvolveYlmDW0, SHConvolveDE0, coord, position.xy);
                        accum += ConvolveSample(SHConvolveYlmDW1, SHConvolveDE1, coord, position.xy);
                    }
                }

                glsl_FragColor0 = float4(accum, 0.0);
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ConvertHemisphere">
            <state name="vertex" value="ConvertHemisphereVS"/>
            <state name="fragment" value="ConvertHemispherePS"/>

            <state name="cullmode" value="none"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="ProjectDualParaboloidToSH">
            <state name="vertex" value="ProjectDualParaboloidToSHVS"/>
            <state name="fragment" value="ProjectDualParaboloidToSHPS"/>

            <state name="cullmode" value="none"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
    </technique>
</effect>