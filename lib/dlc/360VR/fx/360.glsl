<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matModelInverseTranspose" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject"/>
    <parameter name="timer" semantic="Time" />
    <parameter name="decal" type="sampler2D"/>
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matModelInverseTranspose;
            uniform mat4 matViewProject;

            out vec4 normal;
            out vec2 uv;

            void SkyVS()
            {
                uv = glsl_Texcoord.xy;
                normal = matModelInverseTranspose * glsl_Normal;
                gl_Position = matViewProject * matModel * glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            uniform float timer;

            uniform sampler2D decal;

            in vec2 uv;
            in vec4 normal;

            void SkyPS()
            {
                float dayTimer = timer / 50;
                dayTimer -= int(dayTimer);

                glsl_FragColor0 = texture2D(decal, vec2(uv.x, uv.y));
                glsl_FragColor1 = vec4(normalize(normal).xyz, 32);
            }
        ]]>
    </shader>
    <technique name="background">
        <pass name="gbuffer">
            <state name="vertex" value="SkyVS"/>
            <state name="fragment" value="SkyPS"/>
            <state name="cullmode" value="front"/>
        </pass>
    </technique>
</effect>