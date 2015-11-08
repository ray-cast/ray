<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matModelInverseTranspose" semantic="matModelInverseTranspose" />
    <parameter name="matView" semantic="matView"/>
    <parameter name="matViewProject" semantic="matViewProject"/>
    <parameter name="diffuse" type="float4" />
    <parameter name="specular" type="float4" />
    <parameter name="shininess" type="float" />
    <parameter name="opacity" type="float"/>
    <parameter name="texDiffuse" type="sampler2D"/>
    <parameter name="texSpecular" type="sampler2D" />
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matModelInverseTranspose;
            uniform mat4 matView;
            uniform mat4 matViewProject;

            out vec4 normal;
            out vec2 coord;

            void DiffuseVS()
            {
                coord = glsl_Texcoord.xy;
                normal = vec4(mat3(matView) * (matModelInverseTranspose * glsl_Normal).rgb, 1.0);
                gl_Position = matViewProject * matModel * glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            uniform float opacity;
            uniform float shininess;
            uniform vec4 diffuse;
            uniform vec4 specular;

            uniform sampler2D texDiffuse;
            uniform sampler2D texSpecular;

            in vec2 coord;
            in vec4 normal;

            void DiffusePS()
            {
                vec4 albedo;
                vec3 spec;

                if (diffuse.w > 0.0)
                {
                    albedo = diffuse;
                    albedo.a = opacity;
                }
                else
                {
                    albedo = texture(texDiffuse, coord);
                }

                if (specular.w > 0.0)
                {
                    spec = specular.rgb;
                }
                else
                {
                    spec = texture(texSpecular, coord).rgb;
                }

                glsl_FragColor0 = StoreGBufferRT0(albedo, spec);
                glsl_FragColor1 = StoreGBufferRT1(normalize(normal.xyz), shininess);
            }
        ]]>
    </shader>
    <technique name="transparent">
        <pass name="transparent">
            <state name="vertex" value="DiffuseVS"/>
            <state name="fragment" value="DiffusePS"/>
        </pass>
    </technique>
</effect>