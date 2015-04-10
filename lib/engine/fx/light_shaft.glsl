<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <pass name="light_shaft">
        <state name="zwriteenable" value="false"/>
        <state name="zenable" value="false"/>
        <shader type="vertex">
            <![CDATA[
                #version 330 core

                layout(location = 0) in vec4 position;

                out vec2 UV;

                void main()
                {
                    UV = position.xy * 0.5 + 0.5;
                    gl_Position = position;
                }
            ]]>
        </shader>
        <shader type="fragment">
            <![CDATA[
                #version 330

                layout(location = 0) out vec3 oColor;

                in vec2 UV;

                uniform vec2 position; // light project position
                uniform sampler2D scene_texture;

                const int sample_number = 50;
                const float decay    = 0.995;
                const float density  = 1.0;
                const float weight   = 1.0;
                const float exposure = 1 / sample_number;

            /*vec3 caculateRadial(vec2 uv, int sampleNumber, float starstart = 1.0, float starscale = 0.9)
            {
                vec3 color = vec3(0);
                float scale = starstart;

                for(int i = 0; i < sampleNumber; ++i)
                {
                    vec2 texcoord = (uv - 0.5) * scale + 0.5;
                    color += texture2D(bloom, texcoord).rgb;
                    scale *= starscale;
                }

                return color * (1 / sampleNumber);
            }*/

                void main()
                {
                    vec2 delta = vec2(UV - position) * (1 / sample_number) * density;

                    float illuminationDecay = 1.0;

                    vec3 color = vec3(0.0, 0.0, 0.0);
                    vec2 texcoord = UV;

                    for (int i = 0; i < sample_number ; i++)
                    {
                        texcoord -= delta;

                        vec3 sample = texture2D(scene_texture, texcoord).rgb;
                        sample *= illuminationDecay * weight;
                        color += sample;

                        illuminationDecay *= decay;
                    }

                    oColor = color * exposure;
                }
            ]]>
        </shader>
    </pass>
</effect>