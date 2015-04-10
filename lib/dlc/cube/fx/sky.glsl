<?xml version='1.0'?>
<effect language="glsl">
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matNormal" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="timer" semantic="Timer" />
    <parameter name="decal" type="sampler2D"/>
    <shader type="vertex" name="mainVS">
        <![CDATA[
            #version 330
            precision mediump float;

            layout(location = 0) in vec4 glsl_Vertex;
            layout(location = 1) in vec4 glsl_Normmal;
            layout(location = 2) in vec2 glsl_Texcoord;

            uniform mat4 matModel;
            uniform mat4 matNormal;
            uniform mat4 matViewProject;

            uniform float timer;

            out vec2 uv;
            out vec3 normal;
            out float dayTimer;

            void main()
            {
                dayTimer = timer / 100;
                dayTimer -= int(dayTimer);

                uv = glsl_Texcoord;
                normal = (matNormal * glsl_Normmal).xyz;
                gl_Position = matViewProject * matModel * glsl_Vertex;
            }
        ]]>
    </shader>
    <shader type="fragment" name="mainPS">
        <![CDATA[
            #version 330
            precision mediump float;

            layout(location = 0) out vec4 glsl_FragColor0;
            layout(location = 1) out vec4 glsl_FragColor1;

            in vec3 normal;
            in vec2 uv;
            in float dayTimer;

            uniform sampler2D decal;

            void main()
            {
                glsl_FragColor0.rgba = texture2D(decal, vec2(0.5, uv.t));
                glsl_FragColor1 = vec4(normal, 1);
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="gbuffer">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="mainPS"/>
            <state name="depthwrite" value="true"/>
            <state name="cullmode" value="front"/>
        </pass>
    </technique>
</effect>