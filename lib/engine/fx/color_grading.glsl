<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="texSource" type="sampler2D"/>
    <shader name="vertex">
        <![CDATA[
            out vec2 coord;

            void postprocess()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            in vec2 coord;

            uniform sampler2D texSource;

            void ColorGradingPS()
            {
                glsl_FragColor0 = texture2D(texSource, coord);
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="grading">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="ColorGradingPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front" />

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="zero"/>

            <state name="srgbEnable" value="true"/>
        </pass>
    </technique>
</effect>