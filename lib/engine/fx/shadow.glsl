<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matModel" semantic="matModel" />
    <parameter name="matViewProject" semantic="matViewProject" />
    <parameter name="decal" type="sampler2D" />
    <parameter name="factor" type="float"/>
    <shader name="vertex">
        <![CDATA[
            uniform mat4 matModel;
            uniform mat4 matViewProject;

            out vec4 position;

            void ShadowVS()
            {
                gl_Position = position = matViewProject * matModel * glsl_Position;
            }

            void BlurVS()
            {
                gl_Position = position = glsl_Position;
            }
            ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            uniform float factor;
            uniform sampler2D decal;

            in vec4 position;

            const float offset[4] = float[](0.0, 1.3846153846, 2.3307692308, 3.2307692308);
            const float weight[4] = float[](0.28125f, 0.21875f, 0.109375f, 0.03125f);

            void ShadowPS()
            {
                glsl_FragColor0 = vec4(exp(factor * position.z));
            }

            void BlurxPS()
            {
                vec2 texcoord = position.xy;
                texcoord.xy = texcoord.xy * 0.5 + 0.5;

                vec2 sz = textureSize(decal, 0);

                float color = texture2D(decal, texcoord).r * weight[0];

                for (int i = 1; i < 4; i++)
                {
                    color += texture2D(decal, texcoord + vec2(offset[i] / sz.x, 0.0)).r * weight[i];
                    color += texture2D(decal, texcoord - vec2(offset[i] / sz.x, 0.0)).r * weight[i];
                }

                glsl_FragColor0 = vec4(color);
            }

            void BluryPS()
            {
                vec2 texcoord = position.xy;
                texcoord.xy = texcoord.xy * 0.5 + 0.5;

                vec2 sz = textureSize(decal, 0);

                float color = texture2D(decal, texcoord).r * weight[0];

                for (int i = 1; i < 4; i++)
                {
                    color += texture2D(decal, texcoord + vec2(0.0, offset[i] / sz.y)).r * weight[i];
                    color += texture2D(decal, texcoord - vec2(0.0, offset[i] / sz.y)).r * weight[i];
                }

                glsl_FragColor0 = vec4(color);
            }
      ]]>
    </shader>
    <technique name="custom">
        <pass name="shadow">
            <state name="vertex" value="ShadowVS"/>
            <state name="fragment" value="ShadowPS"/>
        </pass>
        <pass name="blurX">
            <state name="vertex" value="BlurVS"/>
            <state name="fragment" value="BlurxPS"/>
        </pass>
        <pass name="blurY">
            <state name="vertex" value="BlurVS"/>
            <state name="fragment" value="BluryPS"/>
        </pass>
    </technique>
</effect>