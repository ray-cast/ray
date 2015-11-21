<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="texSource" type="sampler2D"/>
    <parameter name="texColorGrading" type="sampler2D"/>
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
            uniform sampler2D texColorGrading;

            void ColorGradingPS()
            {
                float3 rgb = texture2D(texSource, coord).rgb;

                const float DIM = 16;

                rgb.xy = (rgb.xy * (DIM - 1) + 0.5f) / DIM;
                float slice = rgb.z * (DIM - 1);
                float s = fract(slice);
                slice -= s;
                rgb.y = (rgb.y + slice) / DIM;

                float3 col0 = sampleCoord(texColorGrading, rgb.xy).xyz;
                float3 col1 = sampleCoord(texColorGrading, float2(rgb.x, rgb.y + 1.0f / DIM)).xyz;

                glsl_FragColor0.rgb = lerp(col0, col1, s);
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="grading">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="ColorGradingPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="none"/>

            <state name="srgbEnable" value="true"/>
        </pass>
    </technique>
</effect>