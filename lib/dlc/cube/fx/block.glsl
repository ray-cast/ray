<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matModelInverseTranspose" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject"/>
    <parameter name="color" type="float4" />
    <parameter name="shininess" type="float" />
    <parameter name="specular" type="float" />
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matModelInverseTranspose;
            uniform mat4 matViewProject;

            out vec4 normal;

            void BlockVS()
            {
                normal = matModelInverseTranspose * glsl_Normal;
                gl_Position = matViewProject * matModel * glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            uniform vec4 color;
            uniform float shininess;
            uniform float specular;

            in vec4 normal;

            void BlockPS()
            {
                glsl_FragColor0 = color;
                glsl_FragColor1 = StoreGBufferRT1(normalize(normal.xyz), shininess);
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="opaque">
            <state name="vertex" value="BlockVS"/>
            <state name="fragment" value="BlockPS"/>

            <state name="cullmode" value="none" />
        </pass>
    </technique>
</effect>