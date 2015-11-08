<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="distance" type="float2"/>
    <parameter name='decal' type="sampler2D"/>
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matViewProject;

            out float2 coord;

            void FontVS()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = mul(matViewProject, glsl_Position);
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            uniform float2 distance;
            uniform sampler2D decal;

            in float2 coord;

            void FontPS()
            {
                float color = sampleCoord(decal, coord).r * distance.x + distance.y;
                color = clamp(color, 0.0, 1);
                glsl_FragColor0 = float4(color);
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="opaque">
            <state name="vertex" value="FontVS"/>
            <state name="fragment" value="FontPS"/>

            <state name="cullmode" value="none"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>
        </pass>
    </technique>
</effect>