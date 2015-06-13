<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <parameter name="texSource" type="sampler2D" />
    <parameter name="lightPosition" type="float2" />
    <shader type="vertex">
        <![CDATA[
            #version 330 core

            layout(location = 0) in vec4 position;

            out vec2 texcoord;

            void main()
            {
                texcoord = position.xy * 0.5 + 0.5;
                gl_Position = position;
            }
        ]]>
    </shader>
    <shader type="fragment">
        <![CDATA[
            #version 330

            layout(location = 0) out vec3 oColor;

            in vec2 texcoord;

            uniform vec2 lightPosition; //scene space
            uniform sampler2D texSource;

            const int sampleNumber = 50;
            const float decay    = 0.995;
            const float density  = 1.0;
            const float weight   = 1.0;
            const float exposure = 1 / sampleNumber;

            void main()
            {
                vec2 delta = vec2(texcoord - lightPosition) * (1 / sampleNumber) * density;

                float illuminationDecay = 1.0;

                vec2 coord = texcoord;
                vec3 color = texture2D(texSource, coord).rgb;

                for (int i = 0; i < sampleNumber ; i++)
                {
                    coord -= delta;

                    vec3 sample = texture2D(texSource, coord).rgb;
                    sample *= illuminationDecay * weight;
                    color += sample;

                    illuminationDecay *= decay;
                }

                oColor = color;
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="godray">
            <state name="zwriteenable" value="false"/>
            <state name="zenable" value="false"/>

                <state name="blend" value="true" />
                <state name="blendSeparate" value="true" />
                <state name="blendsrc" value="one" />
                <state name="blenddst" value="zero" />
                <state name="blendalphasrc" value="one" />
                <state name="blendalphadst" value="zero" />
        </pass>
    </technique>
</effect>