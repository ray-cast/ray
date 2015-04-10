<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="decal" type="sampler2D" />
    <parameter name="factor" type="float"/>
    <shader type="vertex" name="ShadowVS">
        <![CDATA[
            #version 330

            layout(location = 0) in vec4 glsl_Position;

            uniform mat4 matModel;
            uniform mat4 matViewProject;

            out vec4 position;

            void main()
            {
                gl_Position = position = matViewProject * matModel * glsl_Position;
            }
        ]]>
    </shader>
    <shader type="fragment" name="ShadowPS">
        <![CDATA[
            #version 330

            layout(location = 0) out float glsl_FragColor0;

            in vec4 position;

            uniform float factor;

            void main()
            {
                glsl_FragColor0 = exp(factor * position.z);
            }
        ]]>
    </shader>
    <shader type="vertex" name="BlurVS">
        <![CDATA[
            #version 330 core

            layout(location = 0) in vec4 glsl_Position;

            out vec4 position;

            void main()
            {
                gl_Position = position = glsl_Position;
            }
        ]]>
    </shader>
    <shader type="fragment" name="BlurX_PS">
        <![CDATA[
            #version 330 core

            layout(location = 0) out float glsl_FragColor0;

            uniform sampler2D decal;

            in vec4 position;

            const float offset[4] = float[](0.0, 1.3846153846, 2.3307692308, 3.2307692308);
            const float weight[4] = float[](0.28125f, 0.21875f, 0.109375f, 0.03125f);

            void main()
            {
                vec2 texcoord = position.xy;
                texcoord.xy = texcoord.xy * 0.5 + 0.5;

                vec2 sz = textureSize(decal, 0);

                glsl_FragColor0 = texture2D(decal, texcoord).r * weight[0];

                for (int i = 1; i < 4; i++)
                {
                    glsl_FragColor0 += texture2D(decal, texcoord + vec2(offset[i] / sz.x, 0.0)).r * weight[i];
                    glsl_FragColor0 += texture2D(decal, texcoord - vec2(offset[i] / sz.x, 0.0)).r * weight[i];
                }
            }
      ]]>
    </shader>
    <shader type="fragment" name="BlurY_PS">
        <![CDATA[
            #version 330 core

            layout(location = 0) out float glsl_FragColor0;

            uniform sampler2D decal;

            in vec4 position;

            const float offset[4] = float[](0.0, 1.3846153846, 2.3307692308, 3.2307692308);
            const float weight[4] = float[](0.28125f, 0.21875f, 0.109375f, 0.03125f);

            void main()
            {
                vec2 texcoord = position.xy;
                texcoord.xy = texcoord.xy * 0.5 + 0.5;

                vec2 sz = textureSize(decal, 0);

                glsl_FragColor0 = texture2D(decal, texcoord).r * weight[0];

                for (int i = 1; i < 4; i++)
                {
                    glsl_FragColor0 += texture2D(decal, texcoord + vec2(0.0, offset[i] / sz.y)).r * weight[i];
                    glsl_FragColor0 += texture2D(decal, texcoord - vec2(0.0, offset[i] / sz.y)).r * weight[i];
                }
            }
      ]]>
    </shader>
    <technique name="shadow">
        <pass name="shadow">
            <state name="vertex" value="ShadowVS"/>
            <state name="fragment" value="ShadowPS"/>
        </pass>
        <pass name="blurX">
            <state name="vertex" value="BlurVS"/>
            <state name="fragment" value="BlurX_PS"/>
        </pass>
        <pass name="blurY">
            <state name="vertex" value="BlurVS"/>
            <state name="fragment" value="BlurY_PS"/>
        </pass>
    </technique>
</effect>