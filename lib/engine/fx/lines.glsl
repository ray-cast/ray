<?xml version='1.0'?>
<effect language="glsl">
    <parameter name="matProject" semantic="matProject" />
    <shader>
        <![CDATA[
            uniform mat4 matProject;

            varying vec4 diffuse;

#ifdef SHADER_API_VERTEX
            void LineVS()
            {
                diffuse = glsl_Diffuse;
                gl_Position = matProject * glsl_Position;
            }
#endif

#ifdef SHADER_API_FRAGMENT
            void LinePS()
            {
                gl_FragColor = diffuse;
            }
#endif
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="gbuffer">
            <state name="vertex" value="LineVS"/>
            <state name="fragment" value="LinePS"/>

            <state name="blend" value="true"/>
        </pass>
    </technique>
</effect>