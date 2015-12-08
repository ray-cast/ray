<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <include name="sys:fx/lighting.glsl"/>
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matView" semantic="matView" />
    <parameter name="matViewInverse" semantic="matViewInverse" />
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="matProjectInverse" semantic="matProjectInverse" />
    <parameter name="matViewProjectInverse" semantic="matViewProjectInverse" />
    <parameter name="texMRT0" semantic="DeferredGraphicMap" />
    <parameter name="texMRT1" semantic="DeferredNormalMap" />
    <parameter name="texDepth" semantic="DeferredDepthMap" />
    <parameter name="texDepthLinear" semantic="DeferredDepthLinearMap" />
    <parameter name="texLight" semantic="DeferredLightMap" />
    <parameter name="texEnvironmentMap" type="sampler2D"/>
    <parameter name="eyePosition" type="float3" />
    <parameter name="clipInfo" type="float4"/>
    <parameter name="projInfo" type="float4"/>
    <parameter name="lightColor" type="float3" />
    <parameter name="lightDirection" type="float3" />
    <parameter name="lightPosition" type="float3" />
    <parameter name="lightAttenuation" type="float3"/>
    <parameter name="lightRange" type="float"/>
    <parameter name="lightIntensity" type="float"/>
    <parameter name="lightSpotInnerCone" type="float"/>
    <parameter name="lightSpotOuterCone" type="float"/>
    <parameter name="shadowChannel" type="int" />
    <parameter name="shadowMap" type="sampler2D" />
    <parameter name="shadowArrayMap" type="sampler2D" />
    <parameter name="shadowFactor" type="float"/>
    <parameter name="shadowMatrix" type="mat4" />
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matViewProject;

            out float4 position;
            out float2 coord;

            uniform float3 lightDirection;
            uniform float lightRange;

            void DeferredDepthOnlyVS()
            {
                gl_Position = mul(matViewProject, mul(matModel, glsl_Position));
            }

            void DeferredDepthLinearVS()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = position = glsl_Position;
            }

            void DeferredSunLightVS()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = position = glsl_Position;
            }

            void DeferredDirectionalLightVS()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = position = glsl_Position;
            }

            void DeferredAmbientLightVS()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = position = glsl_Position;
            }

            void DeferredSpotLightVS()
            {
                float3 vertex = glsl_Position.xyz * lightRange + lightDirection * lightRange;
                gl_Position = position = matViewProject * matModel * float4(vertex, 1.0);
            }

            void DeferredPointLightVS()
            {
                gl_Position = position = matViewProject * matModel * float4(glsl_Position.xyz * lightRange, 1.0);
            }

            void DeferredShadingVS()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            //#extension GL_NV_shadow_samplers_cube : enable

            uniform mat4 matView;
            uniform mat4 matViewInverse;
            uniform mat4 matProjectInverse;
            uniform mat4 matViewProjectInverse;

            uniform float3 eyePosition;
            uniform float4 clipInfo;
            uniform float4 projInfo;

            uniform float3 lightColor;
            uniform float3 lightPosition;
            uniform float3 lightDirection;
            uniform float3 lightAttenuation;

            uniform float lightIntensity;
            uniform float lightSpotInnerCone;
            uniform float lightSpotOuterCone;

            uniform sampler2D texMRT0;
            uniform sampler2D texMRT1;
            uniform sampler2D texDepth;
            uniform sampler2D texDepthLinear;
            uniform sampler2D texLight;
            uniform samplerCube texEnvironmentMap;

            uniform float shadowFactor;
            uniform mat4 shadowMatrix;
            uniform sampler2D shadowMap;
            uniform sampler2DArray shadowArrayMap;

            in float4 position;
            in float2 coord;

            void DeferredDepthLinearPS()
            {
                float d = sampleCoord(texDepth, coord).r;
                glsl_FragColor0.r = clipInfo.x / (clipInfo.z - clipInfo.y * d);
            }

            void DeferredSunLightPS()
            {
                float4 MRT1 = sampleCoord(texMRT1, coord);
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float roughness = restoreRoughness(MRT1);
                float specular = restoreSpecular(MRT0);

                float3 N = restoreNormal(MRT1);
                float3 P = restorePosition(texDepthLinear, projInfo, coord);

                float3 V = normalize(eyePosition - P);
                float3 L = normalize(lightDirection);

                float4 lighting;
                lighting.rgb = lightColor * brdfLambert(N, L);
                lighting.a = brdfSpecular(N, L, V, roughness, specular);

                glsl_FragColor0 = lighting;
            }

            void DeferredSunLightShadowPS()
            {
                float4 MRT1 = sampleCoord(texMRT1, coord);
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float roughness = restoreRoughness(MRT1);
                float specular = restoreSpecular(MRT0);

                float3 N = restoreNormal(MRT1);
                float3 P = restorePosition(texDepthLinear, projInfo, coord);
                float3 V = normalize(eyePosition - P);
                float3 L = normalize(lightDirection);

                float4 world = mul(matViewInverse, float4(P, 1.0));
                world /= world.w;

                float4 lighting;
                lighting.rgb = lightColor * brdfLambert(N, L);
                lighting.a = brdfSpecular(N, L, V, roughness, specular);
                lighting *= shadowLighting(shadowMap, shadowMatrix, shadowFactor, world.xyz);

                glsl_FragColor0 = lighting;
            }

            void DeferredDirectionalLightPS()
            {
                float4 MRT1 = sampleCoord(texMRT1, coord);
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float roughness = restoreRoughness(MRT1);
                float specular = restoreSpecular(MRT0);

                float3 N = restoreNormal(MRT1);
                float3 P = restorePosition(texDepthLinear, projInfo, coord);

                float3 V = normalize(eyePosition - P);
                float3 L = normalize(lightDirection);

                float4 lighting;
                lighting.rgb = lightColor * brdfLambert(N, L);
                lighting.a = brdfSpecular(N, L, V, roughness, specular);

                glsl_FragColor0 = lighting;
            }

            void DeferredDirectionalLightShadowPS()
            {
                float4 MRT1 = sampleCoord(texMRT1, coord);
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float roughness = restoreRoughness(MRT1);
                float specular = restoreSpecular(MRT0);

                float3 N = restoreNormal(MRT1);
                float3 P = restorePosition(texDepthLinear, projInfo, coord);
                float3 V = normalize(eyePosition - P);
                float3 L = normalize(lightDirection);

                float4 world = matViewInverse * float4(P, 1.0);
                world = world / world.w;

                float4 lighting;
                lighting.rgb = lightColor * brdfLambert(N, L);
                lighting.a = brdfSpecular(N, L, V, roughness, specular);
                lighting *= shadowLighting(shadowMap, shadowMatrix, shadowFactor, world.xyz);

                glsl_FragColor0 = lighting;
            }

            void DeferredSpotLightPS()
            {
                float4 MRT1 = sampleCoord(texMRT1, coord);
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float roughness = restoreRoughness(MRT1);
                float specular = restoreSpecular(MRT0);

                float3 N = restoreNormal(MRT1);
                float3 P = restorePosition(texDepthLinear, projInfo, coord);
                float3 V = normalize(eyePosition - P);
                float3 L = normalize(lightPosition - P);

                float4 lighting;
                lighting.rgb = lightColor * brdfLambert(N, L);
                lighting.a = brdfSpecular(N, L, V, roughness, specular);
                lighting *= spotLighting(lightPosition, P, lightDirection, lightSpotInnerCone, lightSpotOuterCone, lightAttenuation);

                glsl_FragColor0 = lighting;
            }

            void DeferredPointLightPS()
            {
                float4 MRT1 = sampleCoord(texMRT1, coord);
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float roughness = restoreRoughness(MRT1);
                float specular = restoreSpecular(MRT0);

                float3 N = restoreNormal(MRT1);
                float3 P = restorePosition(texDepthLinear, projInfo, coord);
                float3 V = normalize(eyePosition - P);
                float3 L = normalize(lightPosition - P);

                float4 lighting;
                lighting.rgb = lightColor * brdfLambert(N, L);
                lighting.a = brdfSpecular(N, L, V, roughness, specular);
                lighting *= pointLighting(lightPosition, P, lightAttenuation);

                glsl_FragColor0 = lighting;
            }

            void DeferredAmbientLightPS()
            {
                float4 MRT1 = sampleCoord(texMRT1, coord);
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float roughness = restoreRoughness(MRT1);
                float specular = restoreSpecular(MRT0);

                float3 N = mat3(matViewInverse) * restoreNormal(MRT1);
                float3 P = restorePosition(texDepth, matViewProjectInverse, coord);
                float3 V = normalize(eyePosition - P);

                float4 lighting;
                lighting.rgb = lightColor;
                lighting.a = brdfEnvironmentSpecular(N, V, roughness, specular);

                glsl_FragColor0 = lighting;
            }

            void DeferredShadingOpaquesPS()
            {
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float3 color = restoreRGB(texMRT0, coord);
                float4 light = sampleCoord(texLight, coord);

                color = color * light.rgb + light.rgb * light.a;

                glsl_FragColor0 = float4(color, 1.0);
            }

            void DeferredShadingTransparentsPS()
            {
                float4 MRT0 = sampleCoord(texMRT0, coord);

                float3 color = restoreRGB(texMRT0, coord);
                float4 light = sampleCoord(texLight, coord);

                color = color * light.rgb + light.rgb * light.a;

                glsl_FragColor0 = float4(color, 1.0);
            }
        ]]>
    </shader>
    <technique name="custom">
        <pass name="DeferredDepthOnly">
            <state name="vertex" value="DeferredDepthOnlyVS"/>
        </pass>
        <pass name="DeferredDepthLinear">
            <state name="vertex" value="DeferredDepthLinearVS"/>
            <state name="fragment" value="DeferredDepthLinearPS"/>

            <state name="cullmode" value="none"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
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
        <pass name="DeferredDirectionalLight">
            <state name="vertex" value="DeferredDirectionalLightVS"/>
            <state name="fragment" value="DeferredDirectionalLightPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="none"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredDirectionalLightShadow">
            <state name="vertex" value="DeferredDirectionalLightVS"/>
            <state name="fragment" value="DeferredDirectionalLightShadowPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="none"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
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
        <pass name="DeferredSpotLight">
            <state name="vertex" value="DeferredSpotLightVS"/>
            <state name="fragment" value="DeferredSpotLightPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="cullmode" value="none"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="equal"/>
        </pass>
        <pass name="DeferredAmbientLight">
            <state name="vertex" value="DeferredAmbientLightVS"/>
            <state name="fragment" value="DeferredAmbientLightPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="cullmode" value="none"/>

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