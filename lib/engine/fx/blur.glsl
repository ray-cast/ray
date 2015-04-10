    <pass name="blurh">
        <shader type="vertex">
            <![CDATA[
                #version 330 core

                layout(location = 0) in vec4 position;
                layout(location = 7) in vec2 texcoord;

                out vec2 UV;

                void main()
                {
                    UV = texcoord;
                    gl_Position = position;
                }
            ]]>
        </shader>
        <shader type="fragment">
            <![CDATA[
                #version 330 core

                #extension GL_EXT_texture_array : enable

                layout(location = 0) out float oColor0;

                uniform int index;
                uniform sampler2DArray decal;

                in vec2 UV;

                const float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
                const float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

                void main()
                {
                    vec2 size = vec2(textureSize(decal, 0));

                    oColor0 = texture2DArray(decal, vec3(UV, index)).r * weight[0];

                    for (int i = 1; i < 3; ++i)
                    {
                        oColor0 += texture2DArray(decal, vec3(UV + vec2(offset[i], 0.0) / size.x, index)).r * weight[i];
                        oColor0 += texture2DArray(decal, vec3(UV - vec2(offset[i], 0.0) / size.x, index)).r * weight[i];
                    }
                }
          ]]>
        </shader>
        <shader type="vertex">
            <![CDATA[
                #version 330 core

                layout(location = 0) in vec4 position;
                layout(location = 7) in vec2 texcoord;

                out vec2 UV;

                void main()
                {
                    UV = texcoord;
                    gl_Position = position;
                }
            ]]>
        </shader>
        <shader type="fragment">
            <![CDATA[
                #version 330 core

                #extension GL_EXT_texture_array : enable

                layout(location = 0) out float oColor0;

                uniform int index;
                uniform sampler2DArray decal;

                in vec2 UV;

                const float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
                const float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

                void main()
                {
                    vec2 size = vec2(textureSize(decal, 0));

                    oColor0 = texture2DArray(decal, vec3(UV, index)).r * weight[0];

                    for (int i = 1; i < 3; ++i)
                    {
                        oColor0 += texture2DArray(decal, vec3(UV + vec2(0.0, offset[i]) / size.y, index)).r * weight[i];
                        oColor0 += texture2DArray(decal, vec3(UV - vec2(0.0, offset[i]) / size.y, index)).r * weight[i];
                    }
                }
            ]]>
        </shader>
    </pass>