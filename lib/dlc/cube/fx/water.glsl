<?xml version='1.0'?>
<effect language="glsl">
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matModelInverseTranspose" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject"/>
    <parameter name="color" type="float4" />
    <parameter name="shininess" type="float" />
    <parameter name="specular" type="float" />
    <shader>
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matModelInverseTranspose;
            uniform mat4 matViewProject;

            uniform vec4 color;
            uniform float specular;
            uniform float shininess;
            uniform sampler2D decal;

            varying vec4 normal;

#if SHADER_API_VERTEX
            void WaterVS()
            {
                normal = matModelInverseTranspose * glsl_Normal;
                gl_Position = matViewProject * matModel * glsl_Position;
            }
#endif

#if SHADER_API_FRAGMENT
            void WaterPS()
            {
                glsl_FragColor0.rgba = color;
                glsl_FragColor1.rgb = normalize(normalize(normal).xyz);
                glsl_FragColor1.a = shininess + specular * 0.1;
            }
#endif
        ]]>
    </shader>
    <technique name="transparent">
        <pass name="gbuffer">
            <state name="vertex" value="WaterVS"/>
            <state name="fragment" value="WaterPS"/>
        </pass>
    </technique>
</effect>