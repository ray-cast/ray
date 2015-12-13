<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="projScale" type="float"/>
    <parameter name="projInfo" type="float4"/>
    <parameter name="radius" type="float"/>
    <parameter name="radius2" type="float"/>
    <parameter name="sphere[0]" type="float2[]"/>
    <parameter name="bias" type="float"/>
    <parameter name="intensity" type="float"/>
    <parameter name="blurFactor" type="float" />
    <parameter name="blurSharpness" type="float" />
    <parameter name="blurRadius" type="int" />
    <parameter name="blurDirection" type="float2"/>
    <parameter name="texDepth" semantic="DeferredDepthLinearMap" />
    <parameter name="texNormal" semantic="NormalMap" />
    <parameter name="texOcclusion" type="sampler2D" />
    <parameter name="texSource" type="sampler2D"/>
    <shader name="vertex">
        <![CDATA[
            out float2 coord;

            void postprocess()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            #define NUM_SAMPLE 10
            #define NUM_SAMPLE_INV (1.0f / float(NUM_SAMPLE))

            in float2 coord;

            uniform float radius;
            uniform float radius2;
            uniform float projScale;
            uniform float bias;
            uniform float intensity;
            uniform float4 projInfo;
            uniform float2 sphere[NUM_SAMPLE];

            uniform int blurRadius;
            uniform float blurFactor;
            uniform float blurSharpness;
            uniform float2 blurDirection;

            uniform sampler2D texDepth;
            uniform sampler2D texNormal;
            uniform sampler2D texOcclusion;
            uniform sampler2D texSource;

            float linearizeDepth(float2 uv)
            {
                return sampleCoord(texDepth, uv).r;
            }

            float3 samplePosition(float2 uv)
            {
                float d = linearizeDepth(uv);
                return float3(projInfo.xy * uv + projInfo.zw, 1.0) * d;
            }

            float3 sampleNormal(float2 uv)
            {
                float4 normal = sampleCoord(texNormal, uv);
                return restoreNormal(normal);
            }

            float2 tapLocation(int sampleIndex, float2 sampleNoise, float radius)
            {
                float cosacosb = sphere[sampleIndex].x * sampleNoise.x;
                float sinasinb = sphere[sampleIndex].y * sampleNoise.y;
                float sinacosb = sphere[sampleIndex].y * sampleNoise.x;
                float cosasinb = sphere[sampleIndex].x * sampleNoise.y;
                return float2(cosacosb-sinasinb, sinacosb+cosasinb) * radius * (float(sampleIndex) * NUM_SAMPLE_INV);
            }

            void aoPS()
            {
                float3 viewPosition = samplePosition(coord);
                float3 viewNormal = sampleNormal(coord);

                int2 ssC = int2(gl_FragCoord.xy);
                float sampleAngle = float(3 * ssC.x ^ ssC.y + ssC.x * ssC.y) * 10.0f;
                float sampleRadius = projScale * radius / viewPosition.z;
                float sampleAmbient = 0.0f;

                float2 sampleNoise = float2(cos(sampleAngle), sin(sampleAngle));

                for (int i = 0; i < NUM_SAMPLE; ++i)
                {
                    float2 sampleOffset = coord + tapLocation(i, sampleNoise, sampleRadius);
                    float3 samplePosition = samplePosition(sampleOffset);
                    float3 sampleDirection = samplePosition - viewPosition;

                    float sampleDepth = linearizeDepth(sampleOffset);
                    float sampleAngle = dot(sampleDirection, viewNormal);
                    float sampleLength2 = dot(sampleDirection, sampleDirection);

                    float f = max(radius2 - sampleLength2, 0.0f);

                    sampleAmbient += max((sampleAngle - sampleDepth * bias) * inversesqrt(sampleLength2), 0.0f);
                }

                sampleAmbient = max(0.0f, 1.0f - sampleAmbient * intensity * NUM_SAMPLE_INV);

                glsl_FragColor0 = float4(sampleAmbient);
            }

            float bilateralfilter(float2 coord, float r, float center_d)
            {
                float d = linearizeDepth(coord);
                float ddiff = (d - center_d) * blurSharpness;
                return exp2(-r * r * blurFactor - ddiff * ddiff);
            }

            void blurPS()
            {
                float center_d = linearizeDepth(coord);

                float total_c = 0.f;
                float total_w = 0.f;

                for (int r = -blurRadius; r < blurRadius; r++)
                {
                    float2 offset = coord + blurDirection * float(r);

                    float bilateralWeight = bilateralfilter(offset, float(r), center_d);

                    total_c += sampleCoord(texSource, offset).r * bilateralWeight;
                    total_w += bilateralWeight;
                }

                glsl_FragColor0 = float4(total_c / total_w);
            }

            void copyPS()
            {
                glsl_FragColor0 = sampleCoord(texOcclusion, coord).rrrr;
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ao">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="aoPS"/>

            <state name="cullmode" value="front"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blur">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="blurPS"/>

            <state name="cullmode" value="front"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="copy">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="copyPS"/>

            <state name="cullmode" value="front"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="zero" />
            <state name="blenddst" value="srcalpha" />
        </pass>
    </technique>
</effect>