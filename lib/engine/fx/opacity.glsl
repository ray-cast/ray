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

            void OpaqueVS()
            {
                coord = glsl_Texcoord.xy;
                normal = vec4(mat3(matView) * (matModelInverseTranspose * glsl_Normal).rgb, 1.0);
                gl_Position = matViewProject * matModel * glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            uniform float shininess;
            uniform vec4 diffuse;
            uniform vec4 specular;

            uniform sampler2D texDiffuse;
            uniform sampler2D texSpecular;

            in vec2 coord;
            in vec4 normal;

            void OpaquePS()
            {
                vec4 albedo;
                vec3 spec;

                if (diffuse.w > 0.0)
                    albedo = diffuse;
                else
                    albedo = texture(texDiffuse, float2(coord.x, 1-coord.y));

                if (specular.w > 0.0)
                    spec = specular.rgb;
                else
                    spec = texture(texSpecular, coord).rgb;

                glsl_FragColor0 = StoreGBufferRT0(RGBA2sRGB(albedo), spec);
                glsl_FragColor1 = StoreGBufferRT1(normalize(normal.xyz), shininess);
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="opaque">
            <state name="vertex" value="OpaqueVS"/>
            <state name="fragment" value="OpaquePS"/>

            <state name="primitive" value="triangle_or_line" />

            <state name="cullmode" value="none"/>
        </pass>
    </technique>
</effect>