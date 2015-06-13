<?xml version='1.0'?>
<effect language="glsl">
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matNormal" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="timer" semantic="Timer" />
    <parameter name="decal" type="sampler2D"/>
    <shader>
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matNormal;
            uniform mat4 matViewProject;

            uniform float timer;

            uniform sampler2D decal;

            varying vec2 uv;
            varying vec3 normal;
            varying float dayTimer;

#if SHADER_API_VERTEX
            void SkyVS()
            {
                dayTimer = timer / 100;
                dayTimer -= int(dayTimer);

                uv = glsl_Texcoord;
                normal = (matNormal * glsl_Normal).xyz;
                gl_Position = matViewProject * matModel * glsl_Position;
            }
#endif

#if SHADER_API_FRAGMENT
            void SKyPS()
            {
                glsl_FragColor0 = texture2D(decal, vec2(0.5, uv.t));
                glsl_FragColor1 = vec4(normal, 32);
            }
#endif
        ]]>
    </shader>
    <technique name="background">
        <pass name="gbuffer">
            <state name="vertex" value="SkyVS"/>
            <state name="fragment" value="SKyPS"/>
            <state name="depthwrite" value="true"/>
            <state name="cullmode" value="front"/>
        </pass>
    </technique>
</effect>