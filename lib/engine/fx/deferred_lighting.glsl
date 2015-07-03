<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <include name="sys:fx/lighting.glsl"/>
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

            void DeferredDepthOnlyVS()
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


            float shadowLighting(vec3 P)
            {
                vec4 proj = shadowMatrix * vec4(P, 1.0);
                proj.xy /= proj.w;
                proj.xy = proj.xy * 0.5 + 0.5;

                if (proj.x < 0 || proj.y < 0 ||
                    proj.x > 1 || proj.y > 1)
                {
                    return vec4(1);
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

            vec4 samplePosition(vec2 coord)
            {
                float depth  = textureLod(texDepth, coord.xy, 0).r * 2.0 - 1.0;
                vec4 result = matViewProjectInverse * vec4(coord * 2.0 - 1.0, depth, 1.0);
                result /= result.w;
                return result;
            }

            vec4 sampleNormal(vec2 coord)
            {
                return texture(texNormal, coord.xy);
            }

            vec4 spotLight(vec3 P, vec3 N, float sp, float sc)
            {
                vec3 L = normalize(lightPosition - P);
                float nl = max(dot(N, L), 0);
                if (nl == 0)
                    return vec4(0,0,0,0);

                vec3 direction = normalize(P - lightPosition);
                float spotFactor = dot(direction, spotDirection);
                float spotLength = distance(P, lightPosition);
                if (spotFactor < spotAngle || (spotRange * 1.414) < spotLength)
                    return vec4(0,0,0,0);

                float attenuation = smoothstep(spotOuterCone, spotInnerCone, spotFactor);

                vec3 V = normalize(eyePosition - P);
                vec3 H = normalize(L + V);

                float lighting = attenuation * BRDF(N, L, V, sp, sc);

                return vec4(lightColor * lighting, 0.0);
            }

            vec4 pointLight(vec3 world, vec3 N, float sp, float sc)
            {
                vec3 L = normalize(lightPosition - world);

                float nl = max(dot(N, L), 0);
                if (nl == 0)
                    return vec4(0,0,0,0);

                float distance = distance(lightPosition, world);
                float attenuation = max(1 - (distance / range), 0);

                vec3 V = normalize(eyePosition - world);
                vec3 H = normalize(L + V);

                float lighting = attenuation * BRDF(N, L, V, sp, sc);

                return vec4(lightColor * lighting, 0.0);
            }

            void DeferredDepthOnlyPS()
            {
            }

            void DeferredSunLightPS()
            {
                vec4 N = sampleNormal(coord.xy);
                vec4 P = samplePosition(coord.xy);

                vec3 V = normalize(eyePosition - P.xyz);
                vec3 L = normalize(lightDirection);
                vec3 lighting = lightColor * BRDF(N.xyz, L, V, floor(N.a), fract(N.a));

                glsl_FragColor0 = vec4(lighting, 1.0);
            }

            void DeferredSunLightShadowPS()
            {
                vec4 N = sampleNormal(coord.xy);
                vec4 P = samplePosition(coord.xy);

                vec3 V = normalize(eyePosition - P.xyz);
                vec3 L = normalize(lightDirection);
                vec3 lighting = lightColor * BRDF(N.xyz, L, V, floor(N.a), fract(N.a)) * shadowLighting(P.xyz);

                glsl_FragColor0 = vec4(lighting, 1.0);
            }

            void DeferredSpotLightPS()
            {
                vec4 N = sampleNormal(coord.xy);
                vec3 P = samplePosition(coord.xy).xyz;

                glsl_FragColor0 = spotLight(P, N.rgb, floor(N.a), fract(N.a));
            }

            void DeferredPointLightPS()
            {
                vec4 N = sampleNormal(coord.xy);
                vec3 P = samplePosition(coord.xy).xyz;

                glsl_FragColor0 = pointLight(P, N.rgb, floor(N.a), fract(N.a));
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
    <technique name="custom">
        <pass name="DeferredDepthOnly">
            <state name="vertex" value="DeferredDepthOnlyVS"/>
            <state name="fragment" value="DeferredDepthOnlyPS"/>
        </pass>
        <pass name="DeferredPointLight">
            <state name="vertex" value="DeferredPointLightVS"/>
            <state name="fragment" value="DeferredPointLightPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="cullmode" value="none"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredSunLight">
            <state name="vertex" value="DeferredSunLightVS"/>
            <state name="fragment" value="DeferredSunLightPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="none"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredSunLightShadow">
            <state name="vertex" value="DeferredSunLightVS"/>
            <state name="fragment" value="DeferredSunLightShadowPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="none"/>

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

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredShadingOpaques">
            <state name="vertex" value="DeferredShadingVS"/>
            <state name="fragment" value="DeferredShadingOpaquesPS"/>

            <state name="cullmode" value="none"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="notequal"/>
            <state name="stencilRef" value="0" />
        </pass>
        <pass name="DeferredShadingTransparents">
            <state name="vertex" value="DeferredShadingVS"/>
            <state name="fragment" value="DeferredShadingTransparentsPS"/>

            <state name="cullmode" value="none"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="srcalpha"/>
            <state name="blenddst" value="invsrcalpha"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="notequal"/>
            <state name="stencilRef" value="0" />
        </pass>
    </technique>
</effect>