<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <parameter name="texDepth" type="sampler2D" />
    <parameter name="texNormal" type="sampler2D" />
    <parameter name="texSource" type="sampler2D" />
    <parameter name="eyePosition" type="CameraPosition"/>
    <parameter name="eyeDirection" type="CameraDirection"/>
    <parameter name="matViewProjectInverse" type="matViewProjectInverse"/>
    <parameter name="fogStart" type="float" />
    <parameter name="fogEnd" type="float" />
    <parameter name="fogColor" type="float3"/>
    <shader type="vertex" name="mainVS">
        <![CDATA[
            #version 330

            layout(location = 0) in vec4 glsl_Position;

            void main()
            {
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader type="fragment" name="mainPS">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;

            uniform sampler2D texDepth;
            uniform sampler2D texNormal;
            uniform sampler2D texSource;

            uniform mat4 matViewProjectInverse;

            uniform float fogStart;
            uniform float fogEnd;
            uniform vec3 fogColor;
            uniform vec3 eyePosition;
            uniform vec3 eyeDirection;

            in vec4 position;

            void main()
            {
                ivec2 ssC = ivec2(gl_FragCoord.xy);

                vec3 position = eyePosition * -2.0;
                vec3 normal = texelFetch(texNormal, ssC, 0).rgb;

                float depth = texelFetch(texDepth, ssC, 0).r;
                vec4 source = texelFetch(texSource, ssC, 0).rgba;

                float z = -50.0625 / (depth - 1.0025);
                float u = z / dot(normal, position);

                vec4 world = position + (u * normal);

                float integral = 0.02 * -(exp(-position.y * 0.01) - exp(-world.y * 0.01));
                float f = u * integral / (position.y - world.y);

                glsl_FragColor0.rgb = source.rgb * (1.0 - exp(-f));
                glsl_FragColor0.a = source.a;
            }
        ]]>
    </shader>
    <technique name="exp">
        <pass>
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="mainPS"/>
        </pass>
    </technique>
</effect>