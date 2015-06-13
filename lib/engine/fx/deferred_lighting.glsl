<?xml version="1.0"?>
<effect version="1270" language="glsl">
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
    <shader>
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matViewProject;
            uniform mat4 matViewProjectInverse;

            uniform vec3 lightColor;
            uniform vec3 lightDirection;
            uniform vec3 lightPosition;
            uniform vec3 lightAmbient;

            uniform float lightRange;
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

            uniform float shadowFactor;
            uniform int  shadowChannel;
            uniform mat4 shadowMatrix;
            uniform sampler2D shadowMap;

            varying vec4 position;
            varying float range;
            varying float spotRange;
            varying vec3 spotDirection;

            #define PIE 3.1415926

            void depthPeeling(vec2 coord)
            {
                float depth = texture2D(texDepth, coord).r;
                if (depth <= gl_FragCoord.z)
                {
                    discard;
                }
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

            vec4 unproject(vec2 P, float depth)
            {
                vec4 result = matViewProjectInverse * vec4(P, depth, 1.0);
                result /= result.w;
                return result;
            }

            vec4 directionalLight(vec3 P, vec3 N, float sp, float sc)
            {
                vec3 L = normalize(lightDirection);
                vec3 V = normalize(eyePosition - P);
                vec3 H = normalize(L + V);

                float nl = max(dot(N, L), 0);
                if (nl <= 0)
                    return vec4(0,0,0,0);

                float d = (sp + 2) / 8 * pow(dot(N, H), sp);
                float f = sc + (1 - sc) * pow(1 - dot(H, lightDirection), 5);
                float k = 2 / sqrt(PIE * (sp + 2));
                float v = 1 / ((nl * (1 - k) + k) * (dot(N, V) * (1 - k) + k));

                float specular = d * f * v;
                float diffuse = (1 - specular) * nl;
                float lighting = (diffuse + specular);

                if (shadowChannel > 0)
                    lighting *= shadowLighting(P);

                return vec4(lightColor * lighting, 0.0);
            }

#if SHADER_API_VERTEX
            void DeferredDepthOhlyVS()
            {
                gl_Position = matViewProject * matModel * glsl_Position;
            }

            void DeferredSunLightVS()
            {
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
                gl_Position = glsl_Position;
            }
#endif

#if SHADER_API_FRAGMENT
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

                float d = (sp + 2) / 8 * pow(dot(N, H), sp) / 4 * PIE;
                float f = sc + (1 - sc) * pow(1 - dot(H, L), 5);
                float k = 2 / sqrt(PIE * (sp + 2));
                float v = 1 / ((dot(N, L) * (1 - k) + k) * (dot(N, V) * (1 - k) + k));

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
                float depth  = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r * 2.0 - 1.0;

                vec4 N = texelFetch(texNormal, ivec2(gl_FragCoord.xy), 0);

                float shininess = floor(N.a);
                float specular = fract(N.a) * 10;

                vec3 world = unproject(position.xy / position.w, depth).xyz;

                glsl_FragColor0 = directionalLight(world, N.rgb, shininess, specular);
            }

            void DeferredSpotLightPS()
            {
                float depth  = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r * 2.0 - 1.0;

                vec4 N = texelFetch(texNormal, ivec2(gl_FragCoord.xy), 0);

                float shininess = floor(N.a);
                float specular = fract(N.a) * 10;

                vec3 world = unproject(position.xy / position.w, depth).xyz;

                glsl_FragColor0 = spotLight(world, N.rgb, shininess, specular);
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

            void DeferredPointLightPS()
            {
                float depth  = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0).r * 2.0 - 1.0;

                vec4 N = texelFetch(texNormal, ivec2(gl_FragCoord.xy), 0);

                float shininess = floor(N.a);
                float specular = fract(N.a) * 10;

                vec3 world = unproject(position.xy / position.w, depth).xyz;

                glsl_FragColor0 = pointLight(world, N.rgb, shininess, specular);
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
#endif
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

            <state name="blend" value="true"/>
            <state name="blendsrc" value="srcalpha"/>
            <state name="blenddst" value="invsrcalpha"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="gequal"/>
            <state name="stencilRef" value="1" />
        </pass>
        <pass name="DeferredShadingTransparents">
            <state name="vertex" value="DeferredShadingVS"/>
            <state name="fragment" value="DeferredShadingTransparentsPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="srcalpha"/>
            <state name="blenddst" value="invsrcalpha"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="gequal"/>
            <state name="stencilRef" value="1" />
        </pass>
    </technique>
</effect>