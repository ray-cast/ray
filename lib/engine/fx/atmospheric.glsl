<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="eyePosition" semantic="CameraPosition" />
    <parameter name="lightDirection" type="float3" />
    <parameter name="invWavelength" type="float3" />
    <parameter name="outerRadius" type="float" />
    <parameter name="outerRadius2" type="float" />
    <parameter name="innerRadius" type="float" />
    <parameter name="innerRadius2" type="float" />
    <parameter name="krESun" type="float" />
    <parameter name="kmESun" type="float" />
    <parameter name="kr4PI" type="float" />
    <parameter name="km4PI" type="float" />
    <parameter name="scaleFactor" type="float" />
    <parameter name="scaleDepth" type="float" />
    <parameter name="scaleOverScaleDepth" type="float" />
    <shader name="vertex">
        <![CDATA[
        uniform vec3 eyePosition;
        uniform vec3 lightDirection;
        uniform float outerRadius;
        uniform float outerRadius2;
        uniform float innerRadius;
        uniform float innerRadius2;
        uniform float krESun;
        uniform float kmESun;
        uniform float kr4PI;
        uniform float km4PI;
        uniform float scaleFactor;
        uniform float scaleDepth;
        uniform float scaleOverScaleDepth;
        uniform vec3 invWavelength;
        uniform mat4 matViewProject;

        #define NUM_SAMPLES 4

        out vec4 position;
        out vec2 coord;
        out vec3 c0;
        out vec3 c1;
        out vec3 d0;
        out vec3 d1;

        float scale(float angle)
        {
            float x = 1.0f - angle;
            return scaleDepth * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.80f + x * 5.25))));
        }

        void groundVS()
        {
            position = vec4(glsl_Position.xyz * innerRadius,  1.0);
            position.y -= innerRadius;

            vec3 world = position.xyz;
            world.y += innerRadius;

            vec3 cameraPosition = eyePosition;
            cameraPosition.y += innerRadius;

            vec3 ray = position.xyz - eyePosition;
            float rayLength = length(ray);
            ray /= rayLength;

            vec3 start = cameraPosition;

            float startDepth = 0;
            float startOffset = 0;

            float cameraLength2 = dot(cameraPosition, cameraPosition);
            if (cameraLength2 >= outerRadius2)
            {
                float B = 2.0 * dot(cameraPosition, ray);
                float C = cameraLength2 - outerRadius2;
                float det = max(0.0, B * B - 4.0 * C);
                float near = 0.5 * (-B - sqrt(det));

                rayLength -= near;

                start = start + ray * near;
                startDepth = exp((innerRadius - outerRadius) / scaleDepth);
            }
            else
            {
                startDepth = exp((innerRadius - length(cameraPosition)) / scaleDepth);
            }

            float cameraAngle = dot(-ray, world) / length(world);
            float cameraScale = scale(cameraAngle);
            float cameraOffset = startDepth * cameraScale;

            float lightAngle = dot(lightDirection, world) / length(world);
            float lightScale = scale(lightAngle);

            // Initialize the scattering loop variables
            float sampleLength = rayLength / NUM_SAMPLES;
            float scaledLength = sampleLength * scaleFactor;

            vec3 sampleRay = ray * sampleLength;
            vec3 samplePoint = start + sampleRay * 0.5f;

            // Now loop through the sample rays
            vec3 sampleColor = vec3(0, 0, 0);

            for(int i = 0; i < NUM_SAMPLES; i++)
            {
                float height = length(samplePoint);
                float depth = exp(scaleOverScaleDepth * (innerRadius - height));

                float scatter = depth * (lightScale + cameraScale) - cameraOffset;

                vec3 attenuate = exp(-scatter * (invWavelength * kr4PI + km4PI));

                sampleColor += attenuate * (depth * scaledLength);
                samplePoint += sampleRay;
            }

            c0 = sampleColor * (invWavelength * krESun + kmESun);
            c1 = sampleColor * 0.25;

            gl_Position = matViewProject * position;
        }

        void skyVS()
        {
            position = vec4(glsl_Position.xyz * outerRadius,  1.0);
            position.y -= innerRadius;

            vec3 world = position.xyz; world.y += (innerRadius);
            vec3 cameraPosition = eyePosition; cameraPosition.y += (innerRadius);
            vec3 ray = position.xyz - eyePosition;
            float rayLength = length(ray);
            ray /= rayLength;

            vec3 start = cameraPosition;
            float startOffset = 0;

            float cameraLength2 = dot(cameraPosition, cameraPosition);
            if (cameraLength2 >= outerRadius2)
            {
                float B = 2.0 * dot(cameraPosition, ray);
                float C = cameraLength2 - outerRadius2;
                float det = max(0.0, B * B - 4.0 * C);
                float near = 0.5 * (-B - sqrt(det));

                rayLength -= near;

                start = start + ray * near;

                float startDepth = exp(-1.0 / scaleDepth);
                float startAngle = dot(ray, start) / outerRadius;

                startOffset = startDepth * scale(startAngle);
            }
            else
            {
                float startDepth = exp(scaleOverScaleDepth * (innerRadius - length(cameraPosition)));
                float startAngle = dot(ray, start) / length(start);

                startOffset = startDepth * scale(startAngle);
            }

            // Initialize the scattering loop variables
            float sampleLength = rayLength / NUM_SAMPLES;
            float scaledLength = sampleLength * scaleFactor;

            vec3 sampleRay = ray * sampleLength;
            vec3 samplePoint = start + sampleRay * 0.5f;

            // Now loop through the sample rays
            vec3 sampleColor = vec3(0, 0, 0);

            for(int i = 0; i < NUM_SAMPLES; i++)
            {
                float height = length(samplePoint);
                float depth = exp(scaleOverScaleDepth * (innerRadius - height));

                float lightAngle = dot(lightDirection, samplePoint) / height;
                float cameraAngle = dot(ray, samplePoint) / height;

                float scatter = startOffset + depth * (scale(lightAngle) - scale(cameraAngle));

                vec3 attenuate = exp(-scatter * (invWavelength * kr4PI + km4PI));

                sampleColor += attenuate * (depth * scaledLength);
                samplePoint += sampleRay;
            }

            c0 = sampleColor * (invWavelength * krESun);
            c1 = sampleColor * kmESun;
            d0 = cameraPosition - world;
            d1 = lightDirection;

            gl_Position = matViewProject * position;
        }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
        const float g = -0.990;
        const float g2 = g * g;

        in vec4 position;
        in vec3 c0;
        in vec3 c1;
        in vec3 d0;
        in vec3 d1;

        void groundPS()
        {
            glsl_FragColor0.rgb = c0 + c1;
        }

        void skyPS()
        {
            float angle = dot(d1, d0) / length(d0);
            float miePhase = 1.5f * ((1.0f - g2) / (2.0 + g2)) * (1.0 + angle*angle) / pow(1.0f + g2 - 2.0*g*angle, 1.5f);

            glsl_FragColor0.rgb = c0 + miePhase * c1;
        }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ground">
            <state name="vertex" value="groundVS"/>
            <state name="fragment" value="groundPS"/>

            <state name="cullmode" value="back" />
        </pass>
        <pass name="sky">
            <state name="vertex" value="skyVS"/>
            <state name="fragment" value="skyPS"/>

            <state name="cullmode" value="front" />
        </pass>
    </technique>
</effect>