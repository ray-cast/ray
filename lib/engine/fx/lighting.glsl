<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <shader name="fragment">
        <![CDATA[
            #define InvLog2 3.32192809489
            #define PIE 3.1415926

            float fresnelSchlick(float specular, float LdotH)
            {
                // https://seblagarde.wordpress.com/2012/06/03/spherical-gaussien-approximation-for-blinn-phong-phong-and-fresnel
                // pow(1 - LdotH, 5)
                // = exp2(-5.55473 * (LdotH * LdotH)- 6.98316 * LdotH))
                // = exp2((-5.55473 * LdotH - 6.98316) * LdotH)
                return specular + (1.0f - specular) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);
            }

            float attenuationTerm(float3 lightPosition, float3 world, float3 atten)
            {
                float3 v = lightPosition - world;
                float d2 = dot(v, v);
                float d = sqrt(d2);
                return 1 / dot(atten, float3(1, d, d2));
            }

            float specularNormalizeFactor(float roughness)
            {
                return (roughness + 2) / (8 * PIE);
            }

            float roughnessTerm(float NdotH, float roughness)
            {
                // pow(dotHN, K)
                // = pow(dotHN, K)
                // = exp((dotHN-1)*(K+0.775))
                // = exp2((dotHN-1)*((K+0.775)/Log(2)))
                return pow(NdotH, roughness);
                // return exp2((NdotH - 1) * ((roughness + 0.775) * InvLog2));
            }

            float geometricShadowingSchlickBeckmann(float NdotV, float k)
            {
                return NdotV / (NdotV * (1.0 - k) + k);
            }

            float geometricShadowingSmith(float roughness, float NdotL, float NdotV, float LdotH)
            {
                float k = 2 / sqrt(PIE * (roughness + 2));
                return geometricShadowingSchlickBeckmann(NdotL, k) * geometricShadowingSchlickBeckmann(NdotV, k);
                // http://www.filmicworlds.com/2014/04/21/optimizing-ggx-shaders-with-dotlh/
                //float k2 = 4 / (PIE * (roughness + 2));
                //float invK2 = 1 - k2;
                //return 1 / (LdotH * LdotH * invK2 + k2);
            }

            float rimLighting(float NdotV, float LdoV)
            {
                return pow(1 - NdotV, 5);// * (1 - LdoV);
            }

            float brdfLambert(float3 N, float3 L)
            {
                float nl = dot(N, L);
                float lambert = nl * 0.5 + 0.5;
                float diffuse = smoothstep(0.0, 1.5, lambert);
                return max(0, nl);
            }

            float brdfSpecular(float3 N, float3 L, float3 V, float gloss, float specular)
            {
                float nl = max(0, dot(N, L));
                if (nl > 0)
                {
                    float3 H = normalize(L + V);

                    float nh = max(0, dot(H, N));
                    float nv = max(0, dot(V, N));
                    float lh = max(0, dot(L, H));
                    float roughness = pow(8192.0f, gloss);

                    float D = roughnessTerm(nh, roughness);
                    float S = specularNormalizeFactor(roughness);
                    float F = fresnelSchlick(specular, lh);
                    float G = geometricShadowingSmith(roughness, nl, nv, lh);

                    return max(0, D * S * F * G * nl);
                }

                return 0;
            }

            float3 brdfEnvironmentDiffuse(float3 prefilteredColor)
            {
                return prefilteredColor;
            }

            float brdfEnvironmentSpecular(sampler2D texLUT, float3 N, float3 V, float roughness, float specular)
            {
                float NdotV = dot(N, V);
                if (NdotV > 0.0)
                {
                    roughness = pow(8192.0f, roughness);
                    roughness = log2(roughness) / 13;
                    roughness = max(0.5f / 16, roughness);

                    vec2 envBRDF = texture(texLUT, vec2(NdotV, 1 - roughness)).rg;

                    return (specular * envBRDF.x + envBRDF.y);
                }

                return 0.0;
            }

            float brdfEnvironmentSpecular(float3 N, float3 V, float roughness, float specular)
            {
                float NdotV = dot(N, V);

                roughness = pow(8192.0f, roughness);
                roughness = log2(roughness) / 13;
                roughness = max(0.5f / 16, roughness);

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
                envBRDF = clamp(envBRDF, 0, 1);

                return specular * envBRDF.x + envBRDF.y;
            }

            float directionLighting(float3 P, float3 N, float3 L, float3 lightAttenuation)
            {
                return max(dot(N, L), 0);
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
                float distance = distance(lightPosition, world);
                float attenuation = max(1 - (distance / atten.y), 0);
                return attenuation;
            }

            float shadowLighting(sampler2D shadowMap, mat4 shadowMatrix, float shadowFactor, float3 P)
            {
                vec4 proj = shadowMatrix * vec4(P, 1.0);
                proj.xyz /= proj.w;
                proj.xy = proj.xy * 0.5 + 0.5;

                if (proj.x < 0 || proj.y < 0 ||
                    proj.x > 1 || proj.y > 1)
                {
                    return 1.0;
                }

                float d1 = sampleCoord(shadowMap, proj.xy).r;
                float d2 = proj.z;
                float d = shadowFactor * (d1 - d2);
                float shadow = clamp(exp(d), 0, 1);

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