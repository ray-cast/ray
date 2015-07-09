<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texSource" type="sampler2D" />
    <parameter name="illuminationPosition" type="float2" />
    <parameter name="illuminationSample" type="float4" />
    <parameter name="cameraRadio" type="float" />
    <shader name="vertex">
        <![CDATA[
            out vec2 texcoord;

            void LightShaftVS()
            {
                texcoord = glsl_Texcoord.xy;
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            in vec2 texcoord;

            uniform float cameraRadio;

            uniform vec2 illuminationPosition; //screen space
            uniform vec4 illuminationSample; // x : number sample; y : inv samples; z : weight w : decay

            uniform sampler2D texSource;
            uniform sampler2D texDepth;

            void LightShaftPS()
            {
                float dist = length((illuminationPosition - texcoord) * vec2(cameraRadio, 1));

                float illuminationNumber = floor(illuminationSample.x * dist) + 1;
                float illuminationInv = 1.0 / illuminationNumber;
                float illuminationWeight = illuminationInv * dist;
                float illuminationDecay = illuminationSample.w;

                vec2 delta = vec2(illuminationPosition - texcoord) * illuminationInv;
                vec2 coord = texcoord;
                vec4 color = vec4(0, 0, 0, 0);

                float decay = 1.0;

                for (int i = 0; i < illuminationNumber; i++)
                {
                    vec4 sample = vec4(texture(texSource, coord).rgb, texture(texDepth, coord).r);
                    sample = sample * decay;

                    color += sample;
                    coord += delta;
                    decay -= illuminationDecay;
                }

                glsl_FragColor0 = color * illuminationWeight;
            }

            void LightShaftCopy()
            {
                vec4 sample = texture2D(texSource, texcoord);

                float alpha = mix(0, 1, sample.w);
                float dist = length((illuminationPosition - texcoord) * vec2(cameraRadio, 1));
                float distDecay = smoothstep(0, 1, dist);

                vec4 color = sample * alpha * (1 - distDecay);
                color.a = mix(0.4 * distDecay, 1, alpha);
                color.a = mix(color.a, 1, distDecay);

                glsl_FragColor0 = color ;
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="scatter">
            <state name="vertex" value="LightShaftVS"/>
            <state name="fragment" value="LightShaftPS"/>

            <state name="cullmode" value="front" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="copy">
            <state name="vertex" value="LightShaftVS"/>
            <state name="fragment" value="LightShaftCopy"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front" />

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one"/>
            <state name="blenddst" value="one"/>
        </pass>
    </technique>
</effect>