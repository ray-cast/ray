<?xml version='1.0'?>
<effect language="glsl">
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matNormal" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject" />
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

            uniform sampler2D decal;

            void main()
            {
                glsl_FragColor0.rgba = vec4(0, 120.0 / 255, 204.0 / 255.0, 0.75);
                glsl_FragColor1.rgb = normalize(normal.xyz);
                glsl_FragColor1.a   = 32.09;
            }
        ]]>
    </shader>
    <technique name="transparent">
        <pass name="gbuffer">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="mainPS"/>
        </pass>
    </technique>
</effect>