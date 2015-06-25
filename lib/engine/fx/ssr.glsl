<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="texColor" semantic="ColorMap"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texNormal" semantic="NormalMap" />
    <parameter name="matView" semantic="matView"/>
    <parameter name="matProject" semantic="matProject" />
    <parameter name="matProjectInverse" semantic="matProjectInverse" />
    <parameter name="eyePosition" semantic="CameraPosition" />
    <shader>
        <![CDATA[
            varying vec4 position;
            varying vec2 coord;

            uniform sampler2D texColor;
            uniform sampler2D texDepth;
            uniform sampler2D texNormal;

            uniform mat4 matView;
            uniform mat4 matProject;
            uniform mat4 matProjectInverse;

            uniform vec3 eyePosition;

            const float rayStep = 0.25;
            const float minRayStep = 0.1;
            const float maxSteps = 200;
            const int numBinarySearchSteps = 5;

#if SHADER_API_VERTEX
            void ssrVS()
            {
                coord = glsl_Texcoord;
                gl_Position = position = glsl_Position;
            }
#endif

#if SHADER_API_FRAGMENT

            vec3 unproject(vec2 P, float depth)
            {
                vec4 result = matProjectInverse * vec4(P, depth, 1.0);
                result /= result.w;
                return result.rgb;
            }

            vec3 binarySearch(vec3 dir, inout vec3 hitCoord, out float dDepth)
            {
                float depth = 0;

                for (int i = 0; i < numBinarySearchSteps; i++)
                {
                    vec4 projected = matProject * vec4(hitCoord, 1.0);
                    projected.xy /= projected.w;
                    projected.xy = projected.xy * 0.5 + 0.5;

                    depth = texture2D(texDepth, projected.xy).r * 2.0 - 1.0;
                    vec3 P = unproject(projected.xy, depth);

                    float dDepth = hitCoord.z - P.z;

                    if (dDepth > 0.0)
                        hitCoord += dir;

                    dir *= 0.5;
                    hitCoord -= dir;
                }

                vec4 projected = matProject * vec4(hitCoord, 1.0);
                projected.xy = projected.xy / projected.w;
                projected.xy = projected.xy * 0.5 + 0.5;

                return vec3(projected.xy, depth);
            }

            bool traceScreenSpaceRay(vec3 dir, inout vec3 hitCoord, out float dDepth)
            {
                for (int i = 0; i < maxSteps; i++)
                {
                    hitCoord += dir;

                    vec4 projected = matProject * vec4(hitCoord, 1.0);
                    projected /= projected.w;
                    projected.xy = projected.xy * 0.5 + 0.5;

                    float d = texture2D(texDepth, projected.xy).r * 2.0 - 1.0;
                    vec3 P = unproject(projected.xy, d);

                    dDepth = hitCoord.z - P.z;

                    if (dDepth < 0.0)
                    {
                        return true;
                    }
                }

                return false;
            }

            void ssrPS()
            {
                vec4 NS = texture2D(texNormal, coord);
                vec4 color = texture2D(texColor, coord);

                float specular = fract(NS.a) * 10;
                if (specular == 0.0)
                {
                    glsl_FragColor0 = vec4(0);
                    return;
                }

                float depth = texture2D(texDepth, coord).r * 2.0 - 1.0;

                vec3 viewNormal = mat3(matView) * NS.xyz;
                vec3 viewPosition = unproject(position.xy, depth);

                vec3 R = normalize(reflect(viewPosition, viewNormal));
                vec3 I = normalize(viewPosition - eyePosition);

                vec3 dir = R * max(minRayStep, -viewPosition.z) * rayStep;

                if (traceScreenSpaceRay(dir, viewPosition, depth))
                {
                    vec3 coords = binarySearch(dir, viewPosition, depth);
                    if (coords.x < 1.0 && coords.y < 1.0)
                    {
                        vec4 reflectionColor = texture2D(texColor, coords.xy);

                        float fresnel = specular + (1.0 - specular) * pow(1.0 - dot(I, NS.rgb), 5.0);

                        glsl_FragColor0 = vec4(reflectionColor.rgb, fresnel);
                        return;
                    }
                }

                glsl_FragColor0 = vec4(0);
            }
#endif
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ssr">
            <state name="vertex" value="ssrVS"/>
            <state name="fragment" value="ssrPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
        </pass>
    </technique>
</effect>