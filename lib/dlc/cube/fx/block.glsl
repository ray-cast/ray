<?xml version='1.0'?>
<effect language="glsl">
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matNormal" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="color" type="float4" />
    <parameter name="shininess" type="float" />
    <parameter name="specular" type="float" />
    <shader type="vertex" name="mainVS">
        <![CDATA[
            #version 330
            precision mediump float;

            layout(location = 0) in vec4 glsl_Vertex;
            layout(location = 1) in vec4 glsl_Normmal;

            uniform mat4 matModel;
            uniform mat4 matNormal;
            uniform mat4 matViewProject;

            out vec4 normal;

            void main()
            {
                normal = matNormal * glsl_Normmal;
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

            in vec4 normal;

            uniform vec4 color;
            uniform float specular;
            uniform float shininess;
            uniform sampler2D decal;

            void main()
            {
                glsl_FragColor0.rgba = color;
                glsl_FragColor1.rgb = normalize(normal.xyz);
                glsl_FragColor1.a = shininess + specular * 0.1;
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="gbuffer">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="mainPS"/>
        </pass>
    </technique>
</effect>