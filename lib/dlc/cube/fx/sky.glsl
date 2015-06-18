<?xml version='1.0'?>
<effect language="glsl">
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matModelInverseTranspose" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject"/>
    <parameter name="timer" semantic="Time" />
    <parameter name="decal" type="sampler2D"/>
    <shader>
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matModelInverseTranspose;
            uniform mat4 matViewInverseTranspose;
            uniform mat4 matViewProject;

            uniform float timer;

            uniform sampler2D decal;

            varying vec2 uv;
            varying vec4 normal;

#if SHADER_API_VERTEX
            void SkyVS()
            {
                uv = glsl_Texcoord;
                normal = matModelInverseTranspose * glsl_Normal;
                gl_Position = matViewProject * matModel * glsl_Position;
            }
#endif

#if SHADER_API_FRAGMENT
            void SKyPS()
            {
                float dayTimer = timer / 50;
                dayTimer -= int(dayTimer);

                glsl_FragColor0 = texture2D(decal, vec2(0.5, uv.t));
                glsl_FragColor1 = vec4(normalize(normal).xyz, 32);
            }
#endif
        ]]>
    </shader>
    <technique name="background">
        <pass name="gbuffer">
            <state name="vertex" value="SkyVS"/>
            <state name="fragment" value="SKyPS"/>
            <state name="cullmode" value="front"/>
        </pass>
    </technique>
</effect>