<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="matViewProjectInverse" semantic="matViewProjectInverse"/>
    <parameter name="texDepth" semantic="DeferredDepthMap" />
    <parameter name="texDiffuse" semantic="DeferredGraphicMap" />
    <parameter name="texNormal" semantic="DeferredNormalMap" />
    <parameter name="texLight" semantic="DeferredLightMap" />
    <parameter name="texOpaques" type="sampler2D" />
    <parameter name="texTransparent" type="sampler2D" />
    <parameter name="lightColor" semantic="LightDiffuse" />
    <parameter name="lightDirection" semantic="LightDirection" />
    <parameter name="lightPosition" semantic="LightPosition" />
    <parameter name="lightRange" semantic="LightRange"/>
    <parameter name="lightIntensity" semantic="LightIntensity"/>
    <parameter name="lightAmbient" semantic="LightAmbient" />
    <parameter name="spotAngle" semantic="LightSpotAngle"/>
    <parameter name="spotInnerCone" semantic="LightSpotInnerCone"/>
    <parameter name="spotOuterCone" semantic="LightSpotOuterCone"/>
    <parameter name="eyePosition" semantic="CameraPosition" />
    <parameter name="shadowChannel" type="int" />
    <parameter name="shadowMap" type="sampler2D" />
    <parameter name="shadowFactor" type="float"/>
    <parameter name="shadowMatrix" type="mat4" />
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matViewProject;

            out vec4 position;
            out vec4 coord;
            out float range;
            out float spotRange;
            out vec3 spotDirection;

            uniform vec3 lightDirection;
            uniform float lightRange;

            void DeferredDepthOhlyVS()
            {
                gl_Position = matViewProject * matModel * glsl_Position;
            }

            void DeferredSunLightVS()
            {
                coord = glsl_Texcoord;
                gl_Position = position = glsl_Position;
            }

            void DeferredSpotLightVS()
            {
                spotRange = lightRange;
                spotDirection = lightDirection;

                vec3 vertex = glsl_Position.xyz * lightRange + lightDirection * lightRange;
                gl_Position = position = matViewProject * matModel * vec4(vertex, 1.0);
            }

            void DeferredPointLightVS()
            {
                range = lightRange;
                gl_Position = position = matViewProject * matModel * vec4(glsl_Position.xyz * lightRange, 1.0);
            }

            void DeferredShadingVS()
            {
                coord = glsl_Texcoord;
                gl_Position = position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            //#extension GL_EXT_texture_array : enable

            uniform mat4 matViewProjectInverse;

            uniform vec3 lightColor;
            uniform vec3 lightPosition;
            uniform vec3 lightDirection;
            uniform vec3 lightAmbient;
            uniform float lightIntensity;

            uniform float spotAngle;
            uniform float spotInnerCone;
            uniform float spotOuterCone;

            uniform vec3 eyePosition;

            uniform sampler2D texDepth;
            uniform sampler2D texBackground;
            uniform sampler2D texLight;
            uniform sampler2D texDiffuse;
            uniform sampler2D texNormal;
            uniform sampler2D texLUT;

            uniform float shadowFactor;
            uniform int  shadowChannel;
            uniform mat4 shadowMatrix;
            uniform sampler2D shadowMap;
            //uniform sampler2DArray shadowArrayMap;

            in vec4 position;
            in vec4 coord;
            in float range;
            in float spotRange;
            in vec3 spotDirection;

            #define PIE 3.1415926

            void depthPeeling(vec2 coord)
            {
                float depth = texture2D(texDepth, coord).r;
                if (depth <= gl_FragCoord.z)
                {
                    discard;
                }
            }

            vec4 unproject(vec2 P, float depth)
            {
                vec4 result = matViewProjectInverse * vec4(P, depth, 1.0);
                result /= result.w;
                return result;
            }

            float shadowLighting(vec3 world)
            {
                vec4 proj = shadowMatrix * vec4(world, 1.0);
                proj.xy /= proj.w;
                proj.xy = proj.xy * 0.5 + 0.5;

                if (proj.x < 0 || proj.y < 0 ||
                    proj.x > 1 || proj.y > 1)
                {
                    return 1;
                }

                float occluder = texture2D(shadowMap, proj.xy).r;
                return clamp(occluder * exp(-shadowFactor * proj.z), 0, 1);
            }

            /*float shadowLighting(int index, vec3 world)
            {
                vec4 proj = shadowMatrix * vec4(world, 1.0);
                proj.xy /= proj.w;
                proj.xy = proj.xy * 0.5 + 0.5;

                if (proj.x < 0 || proj.y < 0 ||
                    proj.x > 1 || proj.y > 1)
                {
                    return 1;
                }

                float occluder = texture2DArray(shadowArrayMap, vec3(proj.xy, index)).r;
                return clamp(occluder * exp(-shadowFactor * proj.z), 0, 1);
            }

            float shadowCubeLighting(vec3 world)
            {
                vec3 L = lightPosition - world;

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

                return shadowLighting(index, world);
            }*/

            float fresnelSchlick(float specular, float LdotH)
            {
                // https://seblagarde.wordpress.com/2012/06/03/spherical-gaussien-approximation-for-blinn-phong-phong-and-fresnel
                // pow(1 - LdotH, 5)
                // = exp2(-5.55473 * (LdotH * LdotH)- 6.98316 * LdotH))
                // = exp2((-5.55473 * LdotH - 6.98316) * LdotH)
                return specular + (1.0f - specular) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);
            }

            float roughnessTerm(float NdotH, float roughness)
            {
                return ((roughness + 2) / 8) * pow(NdotH, roughness);
            }

            float geometricShadowingSchlickBeckmann(float NdotV, float k)
            {
                return NdotV / (NdotV * (1.0 - k) + k);
            }

            float geometricShadowingSmith(float roughnees, float NdotL, float NdotV)
            {
                float k = (roughnees + 1.0) * (roughnees + 1.0) / 8.0;
                return geometricShadowingSchlickBeckmann(NdotL, k) * geometricShadowingSchlickBeckmann(NdotV, k) / (NdotV * NdotL);
            }

            float brdfLambert(vec3 N, vec3 L)
            {
                return max(dot(N, L), 0);
            }

            float brdfCookTorrance(vec3 N, vec3 V, vec3 L, float roughness, float specular)
            {
                float nl = dot(N, L);
                float nv = dot(N, V);
                if (nl > 0.0 && nv > 0.0)
                {
                    vec3 H = normalize(L + V);

                    float nh = max(dot(H, N), 0.0f);
                    float lh = max(dot(L, H), 0.0f);

                    float D = roughnessTerm(nh, roughness);
                    float F = fresnelSchlick(specular, lh);
                    float G = geometricShadowingSmith(roughness, nl, nv);

                    return D * F * G * nl;
                }

                return 0;
            }

            /*float brdfCookTorrance(vec3 N ,vec3 V, vec3 L, float roughness, float specular)
            {
                float nl = dot(N, L);
                float nv = dot(N, V);
                if (nl > 0.0 && nv > 0.0)
                {
                    float low = floor(roughness);
                    float high = ceil(roughness);
                    float fraction = roughness - low;

                    //float prefiltered = mix(specular)
                    vec2 envBRDF = texture2D(texLUT, vec2(nv, roughness)).xy;

                    return specular * (envBRDF.x + envBRDF.y);
                }

                return 0;
            }*/

            vec4 directionalLight(vec3 P, vec3 N, float roughness, float specular)
            {
                vec3 L = normalize(lightDirection);
                vec3 V = normalize(eyePosition - P);

                return vec4(lightColor * (brdfLambert(N, L) + brdfCookTorrance(N, V, L, roughness, specular)), 1.0);
            }

            vec4 spotLight(vec3 P, vec3 N, float sp, float sc)
            {
                vec3 direction = normalize(P - lightPosition);
                float spotFactor = dot(direction, spotDirection);
                float spotLength = distance(P, lightPosition);
                if (spotFactor < spotAngle || (spotRange * 1.414) < spotLength)
                    discard;

                vec3 L = normalize(lightPosition - P);
                vec3 V = normalize(eyePosition - P);
                vec3 H = normalize(L + V);

                float nl = max(dot(N, L), 0);
                if (nl == 0)
                    return vec4(0,0,0,0);

                float attenuation = smoothstep(spotOuterCone, spotInnerCone, spotFactor);

                float specular = 0;

                float nh = dot(N, H);
                if (nh > 0)
                {
                    float k = 2 / sqrt(PIE * (sp + 2));

                    float roughness = (sp + 2) / 8 * pow(nh, sp);
                    float fresnel = sc + (1 - sc) * pow(1 - dot(H, lightDirection), 5);
                    float shadowing = 1 / ((nl * (1 - k) + k) * (dot(N, V) * (1 - k) + k));

                    specular = roughness * fresnel * shadowing;
                }

                float diffuse = nl * (1 - specular);
                float lighting = attenuation * (diffuse + specular);

                return vec4(lightColor * lighting, 0.0);
            }

            vec4 pointLight(vec3 world, vec3 N, float sp, float sc)
            {
                vec3 L = normalize(lightPosition - world);
                vec3 V = normalize(eyePosition - world);
                vec3 H = normalize(L + V);

                float nl = max(dot(N, L), 0);
                if (nl == 0)
                    return vec4(0,0,0,0);

                float distance = distance(lightPosition, world);
                float attenuation = max(1 - (distance / range), 0);

                float d = (sp + 2) / 8 * pow(dot(N, H), sp) / 4 * PIE;
                float f = sc + (1 - sc) * pow(1 - dot(H, L), 5);
                float k = 2 / sqrt(PIE * (sp + 2));
                float v = 1 / ((nl * (1 - k) + k) * (dot(N, V) * (1 - k) + k));

                float specular = d * f * v;
                float diffuse = (1 - specular) * nl;

                float lighting = attenuation * (diffuse + specular);

                return vec4(lightColor * lighting, 0.0);
            }

            void DeferredDepthOhlyPS()
            {
            }

            void DeferredSunLightPS()
            {
                float depth  = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r;

                vec4 N = texelFetch(texNormal, ivec2(gl_FragCoord.xy), 0);

                float roughness = floor(N.a);
                float specular = fract(N.a);

                vec3 world = unproject(position.xy / position.w, depth).xyz;

                glsl_FragColor0 = directionalLight(world, N.rgb, roughness, specular);
            }

            void DeferredSunLightShadowPS()
            {
                float depth  = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r;

                vec4 N = texelFetch(texNormal, ivec2(gl_FragCoord.xy), 0);

                float roughness = floor(N.a);
                float specular = fract(N.a);

                vec3 world = unproject(position.xy / position.w, depth).xyz;

                glsl_FragColor0 = directionalLight(world, N.rgb, roughness, specular) * shadowLighting(world);
            }

            void DeferredSpotLightPS()
            {
                float depth  = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r;

                vec4 N = texelFetch(texNormal, ivec2(gl_FragCoord.xy), 0);

                float roughness = floor(N.a);
                float specular = fract(N.a);

                vec3 world = unproject(position.xy / position.w, depth).xyz;

                glsl_FragColor0 = spotLight(world, N.rgb, roughness, specular);
            }

            void DeferredPointLightPS()
            {
                float depth  = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r;

                vec4 N = texelFetch(texNormal, ivec2(gl_FragCoord.xy), 0);

                float roughness = floor(N.a);
                float specular = fract(N.a);

                vec3 world = unproject(position.xy / position.w, depth).xyz;

                glsl_FragColor0 = pointLight(world, N.rgb, roughness, specular);
            }

            void DeferredShadingOpaquesPS()
            {
                vec4 diffuse = texelFetch(texDiffuse, ivec2(gl_FragCoord.xy), 0);
                vec4 light = texelFetch(texLight, ivec2(gl_FragCoord.xy), 0);

                vec3 color = diffuse.rgb * (lightAmbient + light.rgb);

                glsl_FragColor0 = vec4(color, diffuse.a);
            }

            void DeferredShadingTransparentsPS()
            {
                vec4 diffuse = texelFetch(texDiffuse, ivec2(gl_FragCoord.xy), 0);
                vec4 light = texelFetch(texLight, ivec2(gl_FragCoord.xy), 0);

                vec3 color = diffuse.rgb * (lightAmbient + light.rgb);

                glsl_FragColor0 = vec4(color, diffuse.a);
            }
        ]]>
    </shader>
    <technique name="deferredlight">
        <pass name="DeferredDepthOhly">
            <state name="vertex" value="DeferredDepthOhlyVS"/>
            <state name="fragment" value="DeferredDepthOhlyPS"/>
        </pass>
        <pass name="DeferredPointLight">
            <state name="vertex" value="DeferredPointLightVS"/>
            <state name="fragment" value="DeferredPointLightPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="cullmode" value="front"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredSunLight">
            <state name="vertex" value="DeferredSunLightVS"/>
            <state name="fragment" value="DeferredSunLightPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="cullmode" value="front"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredSunLightShadow">
            <state name="vertex" value="DeferredSunLightVS"/>
            <state name="fragment" value="DeferredSunLightShadowPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredSpotLight">
            <state name="vertex" value="DeferredSpotLightVS"/>
            <state name="fragment" value="DeferredSpotLightPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="cullmode" value="front"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredShadingOpaques">
            <state name="vertex" value="DeferredShadingVS"/>
            <state name="fragment" value="DeferredShadingOpaquesPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="notequal"/>
            <state name="stencilRef" value="0" />
        </pass>
        <pass name="DeferredShadingTransparents">
            <state name="vertex" value="DeferredShadingVS"/>
            <state name="fragment" value="DeferredShadingTransparentsPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="srcalpha"/>
            <state name="blenddst" value="invsrcalpha"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="notequal"/>
            <state name="stencilRef" value="0" />
        </pass>
    </technique>
</effect>