<?xml version='1.0'?>
<effect language="glsl">
    <parameter name="matProject" semantic="matProject" />
    <shader type="vertex" name="mainVS">
        <![CDATA[
            #version 330
            precision mediump float;

            layout(location = 0) in vec4 glsl_Vertex;
            layout(location = 3) in vec4 glsl_Diffuse;

            uniform mat4 matProject;

            out vec4 diffuse;

            void main()
            {
                diffuse = glsl_Diffuse;
                gl_Position = matProject * glsl_Vertex;
            }
        ]]>
    </shader>
    <shader type="fragment" name="mainPS">
        <![CDATA[
            #version 330
            precision mediump float;

            in vec4 diffuse;

            void main()
            {
                gl_FragColor = diffuse;
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="gbuffer">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="mainPS"/>
            <state name="blend" value="true"/>
        </pass>
    </technique>
</effect>