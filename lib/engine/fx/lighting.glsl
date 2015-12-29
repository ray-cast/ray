<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <shader name="fragment">
        <![CDATA[
            #define InvLog2 3.32192809489f
            #define InvPIE 0.318309886142f
            #define InvPIE8 0.039788735767f
            #define InvPIE4 0.079577471535f
            #define PIE 3.141592654f

            float fresnelSchlick(float specular, float LdotH)
            {
                // https://seblagarde.wordpress.com/2012/06/03/spherical-gaussien-approximation-for-blinn-phong-phong-and-fresnel
                // pow(1 - LdotH, 5)
                // = exp2(-5.55473 * (LdotH * LdotH)- 6.98316 * LdotH))
                // = exp2((-5.55473 * LdotH - 6.98316) * LdotH)
                return specular + (1.0f - specular) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);
            }

            float3 fresnelSchlick(float3 specular, float LdotH)
            {
                return specular + (1.0f - specular) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);
            }

            float roughnessTerm(float NdotH, float roughness)
            {
                // https://seblagarde.wordpress.com/2012/06/03/spherical-gaussien-approximation-for-blinn-phong-phong-and-fresnel
                // pow(dotHN, K)
                // = exp(-K*(1-dotHN))
                // = exp2(K / InvLog2 * dotHN - K / InvLog2)
                return (roughness * InvPIE8 + InvPIE4) * exp2(roughness * InvLog2 * NdotH - roughness * InvLog2);
            }

            float geometricShadowingSchlickBeckmann(float NdotV, float k)
            {
                return NdotV / (NdotV * (1.0f - k) + k);
            }

            float geometricShadowingSmith(float gloss, float NdotL, float NdotV, float LdotH)
            {
                // http://www.filmicworlds.com/2014/04/21/optimizing-ggx-shaders-with-dotlh/
                // geometricShadowingSchlickBeckmann(NdotL, k) * geometricShadowingSchlickBeckmann(NdotV, k);
                // = geometricShadowingSchlickBeckmann(LdotH, k) * geometricShadowingSchlickBeckmann(LdotH, k);
                float k = min(1.0f, gloss + 0.545f);
                float k2 = k * k;
                float invK2 = 1.0f - k2;
                return 1.0f / (LdotH * LdotH * invK2 + k2);
            }

            float brdfLambert(float3 N, float3 L)
            {
                float nl = dot(N, L);
                float lambert = mad(nl, 0.5f, 0.5f);
                float diffuse = smoothstep(0.0f, 1.5f, lambert);
                return max(0.0f, nl);
            }

            float brdfSpecular(float3 N, float3 L, float3 V, float gloss, float specular)
            {
                float nl = max(0.0f, dot(N, L));
                if (nl > 0.0f)
                {
                    float3 H = normalize(L + V);

                    float nh = max(0.0f, dot(H, N));
                    float nv = max(0.0f, dot(V, N));
                    float lh = max(0.0f, dot(L, H));
                    float roughness = pow(8192.0f, gloss);

                    float D = roughnessTerm(nh, roughness);
                    float F = fresnelSchlick(specular, lh);
                    float G = geometricShadowingSmith(gloss, nl, nv, lh);

                    return max(0.0f, D * F * G * nl);
                }

                return 0.0f;
            }

            float3 brdfEnvironmentDiffuse(float3 prefilteredColor)
            {
                return prefilteredColor;
            }

            float brdfEnvironmentSpecular(sampler2D texLUT, float3 N, float3 V, float roughness, float specular)
            {
                float NdotV = dot(N, V);
                if (NdotV > 0.0f)
                {
                    roughness = pow(8192.0f, roughness);
                    roughness = log2(roughness) / 13.0f;
                    roughness = max(0.5f / 16.0f, roughness);

                    float2 envBRDF = texture(texLUT, float2(NdotV, 1.0f - roughness)).rg;

                    return (mad(specular, envBRDF.x, envBRDF.y));
                }

                return 0.0f;
            }

            float brdfEnvironmentSpecular(float3 N, float3 V, float roughness, float specular)
            {
                float NdotV = dot(N, V);

                roughness = pow(8192.0f, roughness);
                roughness = log2(roughness) / 13.0f;
                roughness = max(0.5f / 16.0f, roughness);

                float2 envBRDF;
                float4 tmp = ((float4(-4.996914762f, 7.253111161f, -1.963867075f, -0.170416225f) * roughness
                    + float4(8.993475061f, -15.42473953f, 4.712593001f, 0.448102365f)) * roughness
                    + float4(-4.928234727f, 10.95286522f, -3.852980973f, -0.389332014f)) * roughness
                    + float4(0.846124834f, -1.596815751f, 1.005729748f, 0.113484128f);
                envBRDF.x = (((tmp.x * NdotV + tmp.y) * NdotV + tmp.z) * NdotV) + tmp.w;
                tmp = ((float4(-0.684077741f, 1.316163916f, -0.70869252f, 0.073264505f) * roughness
                    + float4(0.719315865f, -1.384932814f, 0.775880046f, -0.105818706f)) * roughness
                    + float4(-0.101034049f, 0.18680998f, -0.117919199f, 0.030727381f)) * roughness
                    + float4(0.001922126f, 0.006697305f, -0.015741592f, 0.007229544f);
                envBRDF.y = (((tmp.x * NdotV + tmp.y) * NdotV + tmp.z) * NdotV) + tmp.w;
                envBRDF = clamp(envBRDF, 0.0f, 1.0f);

                return specular * envBRDF.x + envBRDF.y;
            }

            float rimLighting(float NdotV, float rimPower)
            {
                return (1.0f - NdotV) * rimPower;// * (1 - LdoV);
            }

            float attenuationTerm(float3 lightPosition, float3 world, float3 atten)
            {
                float3 v = lightPosition - world;
                float d2 = dot(v, v);
                float d = sqrt(d2);
                return 1.0f / dot(atten, float3(1.0f, d, d2));
            }

            float skinBeckmann(float ndoth, float m)
            {
                float alpha = acos( ndoth );
                float ta = tan( alpha );
                float val = 1.0f/(m*m*pow(ndoth,4.0f))*exp(-(ta*ta)/(m*m));
                return val;
            }

            float directionLighting(float3 P, float3 N, float3 L, float3 lightAttenuation)
            {
                return max(dot(N, L), 0.0f);
            }

            float spotLighting(float3 spotPosition, float3 P, float3 spotDirection, float spotInnerCone, float spotOuterCone, float3 atten)
            {
                float3 V = normalize(P - spotPosition);
                float spotFactor = dot(V, spotDirection);
                float attenuation = smoothstep(spotOuterCone, spotInnerCone, spotFactor);
                return attenuation * attenuationTerm(spotPosition, P, atten);
            }

            float pointLighting(float3 lightPosition, float3 world, float3 atten)
            {
                float distance = length(lightPosition - world);
                float attenuation = max(1.0f - (distance / atten.y), 0.0f);
                return attenuation;
            }

            float shadowLighting(sampler2D shadowMap, mat4 shadowMatrix, float shadowFactor, float3 P)
            {
                float4 proj = shadowMatrix * float4(P, 1.0f);
                proj.xyz /= proj.w;
                proj.xy = mad(proj.xy, 0.5f, 0.5f);

                if (proj.x < 0.0f || proj.y < 0.0f ||
                    proj.x > 1.0f || proj.y > 1.0f)
                {
                    return 1.0f;
                }

                float d1 = sampleCoord(shadowMap, proj.xy).r;
                float d2 = proj.z;
                float d = shadowFactor * (d1 - d2);
                float shadow = clamp(exp(d), 0.0f, 1.0f);

                return shadow;
            }

            /*float shadowLighting(sampler2DArray shadowMap, mat4 shadowMatrix, float shadowFactor, float3 worldPosition, int index)
            {
                vec4 proj = shadowMatrix * vec4(worldPosition, 1.0);
                proj.xy /= proj.w;
                proj.xy = proj.xy * 0.5 + 0.5;

                if (proj.x < 0 || proj.y < 0 ||
                    proj.x > 1 || proj.y > 1)
                {
                    return 1;
                }

                float d1 = texture2DArray(shadowMap, float3(proj.xy,index)).r;
                float d2 = proj.z;
                float d = shadowFactor * (d1 - d2);
                float shadow = clamp(exp(d), 0, 1);

                return shadow;
            }

            float shadowCubeLighting(sampler2DArray shadowMap, mat4 shadowMatrix, float shadowFactor, float3 worldPosition, float3 lightPosition, float3 world)
            {
                float3 L = lightPosition - world;

                float axis[6];
                axis[0] =  L.x;
                axis[1] = -L.x;
                axis[2] =  L.y;
                axis[3] = -L.y;
                axis[4] =  L.z;
                axis[5] = -L.z;

                int index = 0;
                for (int i = 1; i < 6; i++)
                {
                    if (axis[i] > axis[index])
                    {
                        index = i;
                    }
                }

                return shadowLighting(shadowMap, shadowMatrix, shadowFactor, worldPosition, index);
            }*/
        ]]>
    </shader>
</effect>