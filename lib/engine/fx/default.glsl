<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="scaleY" type="float" />
    <parameter name="decal" type="sampler2D"/>
    <shader name="vertex">
        <![CDATA[
            out float4 diffuse;
            out float2 coord;

            uniform float scaleY;

            void UILayerVS()
            {
                coord = glsl_Texcoord.xy;
                diffuse = glsl_Diffuse;
                float4 vpos = glsl_Position;
                vpos.y *= scaleY;
                gl_Position = vpos;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            in float4 diffuse;
            in float2 coord;

            uniform sampler2D decal;

            void UILayerPS()
            {
                glsl_FragColor0 = sampleCoord(decal, coord) * diffuse;
            }
        ]]>
    </shader>
    <technique name="opaque">
        <pass name="ui">
            <state name="vertex" value="UILayerVS"/>
            <state name="fragment" value="UILayerPS"/>

            <state name="primitive" value="triangle" />

            <state name="cullmode" value="none"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="srcalpha"/>
            <state name="blenddst" value="invsrcalpha"/>
        </pass>
    </technique>
</effect>