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

            out vec4 position;

            void main()
            {
                gl_Position = position = glsl_Position;
            }
        ]]>
    </shader>
    <shader type="fragment" name="mainPS">
        <![CDATA[
            #version 330

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texColor;
            uniform sampler2D texDepth;
            uniform sampler2D texNormal;

            uniform vec3 eyePosition;

            uniform mat4 matView;
            uniform mat4 matProject;
            uniform mat4 matProjectInverse;

            in vec4 position;

            const float rayStep = 0.25;
            const float minRayStep = 0.1;
            const float maxSteps = 20;
            const float searchDist = 5;
            const float searchDistInv = 0.2;
            const int numBinarySearchSteps = 5;
            const float maxDDepth = 1.0;
            const float maxDDepthInv = 1.0;

            vec4 unproject(vec2 P, float depth)
            {
                vec4 result = matProjectInverse * vec4(P, depth, 1.0);
                result /= result.w;
                return result;
            }

            vec2 binarySearch(vec3 dir, inout vec3 hitCoord)
            {
                for (int i = 0; i < maxSteps; i++)
                {
                    vec4 projected = matProject * vec4(hitCoord, 1.0);
                    projected.xyz /= projected.w;
                    projected.xy = projected.xy * 0.5 + 0.5;

                    float depth = texture2D(texDepth, projected.xy).r * 2.0 - 1.0;

                    float dDepth = projected.z - depth;

                    if (dDepth > 0)
                        hitCoord += dir;

                    dir *= 0.5;
                    hitCoord -= dir;
                }

                vec4 projected = matProject * vec4(hitCoord, 1.0);
                projected.xyz /= projected.w;
                projected.xy = projected.xy * 0.5 + 0.5;

                return projected.xy;
            }

            bool traceScreenSpaceRay(vec3 R, inout vec3 hitCoord, out vec2 hitPixel)
            {
                vec3 dir = R * max(minRayStep, -hitCoord.z) * rayStep;

                for (int i = 0; i < maxSteps; i++)
                {
                    hitCoord += dir;

                    vec4 projected = matProject * vec4(hitCoord, 1.0);
                    projected /= projected.w;
                    projected.xy = projected.xy * 0.5 + 0.5;

                    float depth = texture2D(texDepth, projected.xy).r * 2.0 - 1.0;
                    float dDepth = projected.z - depth;

                    if (dDepth > 0.0)
                    {
                        hitPixel = projected.xy;
                        return true;
                    }
                }

                return false;
            }

            void main()
            {
                vec2 coord = position.xy;
                coord = coord * 0.5 + 0.5;

                vec4 NS = texture2D(texNormal, coord);
                vec4 color = texture2D(texColor, coord);

                float specular = fract(NS.a) * 10;
                if (specular == 0)
                {
                    glsl_FragColor0 = color;
                    return;
                }

                float depth = texture2D(texDepth, coord).r * 2.0 - 1.0;

                vec3 N = mat3(matView) * NS.xyz;
                vec3 P = unproject(position.xy, depth).xyz;
                vec3 R = normalize(reflect(P, N));

                vec2 hitPixel;

                traceScreenSpaceRay(R, P, hitPixel);
                if (hitPixel.x < 0 || hitPixel.y < 0 ||
                    hitPixel.x > 1 || hitPixel.y > 1)
                {
                    glsl_FragColor0 = vec4(0);
                    return;
                }

                glsl_FragColor0 = texture2D(texColor, hitPixel);
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