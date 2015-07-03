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

            void WaterVS()
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

            void WaterPS()
            {
                glsl_FragColor0 = color;
                glsl_FragColor1.rgb =  normalize(normalize(normal).xyz);
                glsl_FragColor1.a = shininess + specular * 0.1;
            }
        ]]>
    </shader>
    <technique name="transparent">
        <pass name="transparent">
            <state name="vertex" value="WaterVS"/>
            <state name="fragment" value="WaterPS"/>
        </pass>
    </technique>
</effect>