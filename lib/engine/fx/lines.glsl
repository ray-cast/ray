<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matProject" semantic="matProject" />
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matProject;

            out vec4 diffuse;

            void LineVS()
            {
                diffuse = glsl_Diffuse;
                gl_Position = matProject * glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            in vec4 diffuse;

            void LinePS()
            {
                gl_FragColor = diffuse;
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="lines">
            <state name="vertex" value="LineVS"/>
            <state name="fragment" value="LinePS"/>

            <state name="blend" value="true"/>
        </pass>
    </technique>
</effect>