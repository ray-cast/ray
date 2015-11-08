<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matModelInverseTranspose" semantic="matModelInverseTranspose" />
    <parameter name="matViewProject" semantic="matViewProject"/>
    <parameter name="decal" type="sampler2D"/>
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matModelInverseTranspose;
            uniform mat4 matViewProject;

            out vec4 normal;
            out vec2 uv;

            void SkyVS()
            {
                uv = glsl_Texcoord.xy;
                normal = matModelInverseTranspose * glsl_Normal;
                gl_Position = matViewProject * matModel * glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            uniform sampler2D decal;

            in vec2 uv;
            in vec4 normal;

            void SkyPS()
            {
                glsl_FragColor0 = RGBA2sRGB(sampleSphere(decal, normal.xyz));
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="specific">
            <state name="vertex" value="SkyVS"/>
            <state name="fragment" value="SkyPS"/>

            <state name="cullmode" value="front"/>
        </pass>
    </technique>
</effect>