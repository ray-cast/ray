<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <parameter name="matViewInverseTranspose" semantic="matViewInverseTranspose"/>
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="eyePosition" semantic="CameraPosition" />
    <parameter name="lightDirection" type="float3" />
    <parameter name="invWavelength" type="float3" />
    <parameter name="cameraHeight" type="float" />
    <parameter name="cameraHeight2" type="float" />
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
    <shader>
        <![CDATA[
        uniform vec3 eyePosition;
        uniform vec3 lightDirection;
        uniform float cameraHeight;
        uniform float cameraHeight2;
        uniform float outerRadius;
        uniform float outerRadius2;
        uniform float innerRadius;
        uniform float innerRadius2;
        uniform float scaleFactor;
        uniform float scaleDepth;
        uniform float scaleOverScaleDepth;

        uniform float krESun;
        uniform float kmESun;
        uniform float kr4PI;
        uniform float km4PI;

        uniform vec3 invWavelength;

        uniform mat4 matViewInverseTranspose;
        uniform mat4 matViewProject;

        varying vec2 uv;
        varying vec4 normal;
        varying vec4 position;

        const float g = -0.990;
        const float g2 = g * g;

        #define NUM_SAMPLES 2

        float scale(float fCos)
        {
            float x = 1.0f - fCos;
            return scaleDepth * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.80f + x * 5.25))));
        }

#if SHADER_API_VERTEX
        void AtmosphericVS()
        {
            uv = glsl_Texcoord;
            normal = matViewInverseTranspose * glsl_Normal;
            position = vec4(glsl_Position.xyz * outerRadius,  1.0);
            gl_Position = matViewProject * position;
        }
#endif

#if SHADER_API_FRAGMENT
        void AtmosphericPS()
        {
            vec3 cameraPosition = eyePosition;
            vec3 ray = position.xyz - cameraPosition;
            float rayLength = length(ray);
            ray /= rayLength;

            vec3 start = cameraPosition;
            float startOffset = 0;

            if (cameraHeight >= outerRadius)
            {
                float B = 2.0 * dot(cameraPosition, ray);
                float C = cameraHeight2 - outerRadius2;
                float det = max(0.0, B*B - 4.0 * C);
                float near = 0.5 * (-B - sqrt(det));

                start = start + ray * near;
                rayLength -= near;

                float startDepth = exp(-1.0 / scaleDepth);
                float startAngle = dot(ray, start) / outerRadius;

                startOffset = startDepth * scale(startAngle);
            }
            else
            {
                float startDepth = exp(scaleOverScaleDepth * (innerRadius - cameraHeight));
                float startAngle = dot(ray, position.xyz) / length(position.xyz);

                startOffset = startDepth * scale(startAngle);
            }

            // Initialize the scattering loop variables
            float sampleLength = rayLength / NUM_SAMPLES;
            float scaledLength = sampleLength * scaleFactor;

            vec3 sampleRay = ray * sampleLength;
            vec3 samplePoint = start + sampleRay * 0.5f;

            // Now loop through the sample rays
            vec3 sampleColor = vec3(0, 0, 0);
            vec3 sampleAttenuate = vec3(0, 0, 0);

            for(int i = 0; i < NUM_SAMPLES; i++)
            {
                float height = length(samplePoint);
                float depth = exp(scaleOverScaleDepth * (innerRadius - height));

                float lightAngle = dot(lightDirection, samplePoint) / height;
                float cameraAngle = dot(ray, samplePoint) / height;

                float scatter = startOffset + depth * (scale(lightAngle) - scale(cameraAngle));

                sampleAttenuate = exp(-scatter * (invWavelength * kr4PI + km4PI));

                sampleColor += (depth * scaledLength) * sampleAttenuate;
                samplePoint += sampleRay;
            }

            vec3 direction = cameraPosition - position.xyz;
            vec3 mieColor = sampleColor * kmESun;
            vec3 rayleighColor = sampleColor * (invWavelength * krESun);

            float fCos = dot(lightDirection, direction) / length(direction);
            float rayleighPhase = 0.75f * (1.0f + fCos*fCos);
            float miePhase = 1.5f * ((1.0f - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0f + g2 - 2.0*g*fCos, 1.5f);

            glsl_FragColor0.rgb = rayleighPhase * rayleighColor + mieColor * miePhase;
        }
#endif
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="SAT">
            <state name="vertex" value="AtmosphericVS"/>
            <state name="fragment" value="AtmosphericPS"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>

            <state name="cullmode" value="front" />
        </pass>
    </technique>
</effect>