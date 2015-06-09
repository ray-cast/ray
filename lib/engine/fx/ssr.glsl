<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <parameter name="texColor" semantic="ColorMap"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texNormal" semantic="NormalMap" />
    <parameter name="matView" semantic="matView"/>
    <parameter name="matProject" semantic="matProject" />
    <parameter name="matProjectInverse" semantic="matProjectInverse" />
    <parameter name="eyePosition" semantic="CameraPosition" />
    <shader type="vertex" name="mainVS">
        <![CDATA[
            #version 330 core

            layout(location = 0) in vec4 glsl_Position;
            layout(location = 2) in vec2 glsl_Texcoord;

            out vec4 position;
            out vec2 coord;

            void main()
            {
                coord = glsl_Texcoord;
                gl_Position = position = glsl_Position;
            }
        ]]>
    </shader>
    <shader type="fragment" name="mainPS">
        <![CDATA[
            #version 330

            layout(location = 0) out vec4 glsl_FragColor0;

            in vec2 coord;
            in vec4 position;

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

            void main()
            {
                vec4 NS = texture2D(texNormal, coord);
                vec4 color = texture2D(texColor, coord);

                float specular = fract(NS.a) * 10;
                if (specular == 0.0)
                {
                    glsl_FragColor0 = color;
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

                        glsl_FragColor0 = mix(color, reflectionColor, fresnel);
                        return;
                    }
                }

                glsl_FragColor0 = color;
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ssr">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="mainPS"/>
            <state name="depthtest" value="false"/>
        </pass>
    </technique>
</effect>