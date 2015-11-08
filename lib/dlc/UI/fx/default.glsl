<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matViewProject" semantic="matViewProject" />
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matViewProject;

            out vec4 diffuse;

            void UILayerVS()
            {
                diffuse = glsl_Diffuse;
                gl_Position = matViewProject * glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            in vec4 diffuse;

            void UILayerPS()
            {
                glsl_FragColor0 = diffuse;
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="opaque">
            <state name="vertex" value="UILayerVS"/>
            <state name="fragment" value="UILayerPS"/>

            <state name="primitive" value="triangle_or_line" />

            <state name="cullmode" value="back"/>
        </pass>
    </technique>
</effect>