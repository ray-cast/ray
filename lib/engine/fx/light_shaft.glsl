<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="illuminationSource" type="sampler2D" />
    <parameter name="illuminationPosition" type="float4" />
    <parameter name="illuminationSample" type="float4" />
    <parameter name="illuminationRadio" type="float" />
    <shader name="vertex">
        <![CDATA[
            out float2 coord;

            void LightShaftVS()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            in float2 coord;

            uniform float illuminationRadio;  // camera radio
            uniform float3 illuminationPosition; //screen space
            uniform float4 illuminationSample; // x : number sample; y : inv samples; z : weight w : decay
            uniform sampler2D illuminationSource;
            uniform sampler2D texDepth;

            void LightShaftPS()
            {
                float2 sampleCoord = coord;
                float4 sampleColor = float4(0, 0, 0, 0);
                float2 sampleDecay = float2(1.0, 1.0);

                for (int i = 0; i < illuminationSample.x; i++)
                {
                    sampleCoord = (coord - illuminationPosition.xy) * sampleDecay.x + illuminationPosition.xy;
                    sampleColor += float4(texture(illuminationSource, sampleCoord).rgb, texture(texDepth, coord).r);
                    sampleDecay *= illuminationSample.w;
                }

                glsl_FragColor0 = sampleColor * illuminationSample.z;
            }

            void LightShaftCopy()
            {
                float4 sample = texture2D(illuminationSource, coord);

                float alpha = mix(0, 1, sample.w);
                float dist = length((illuminationPosition.xy - coord) * float2(illuminationRadio, 1));
                float distDecay = smoothstep(0, 1, dist);

                float4 color = sample * alpha * (1 - distDecay);
                color.a = mix(0.4 * distDecay, 1, alpha);
                color.a = mix(color.a, 1, distDecay);

                glsl_FragColor0 = color ;
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="LightScatter">
            <state name="vertex" value="LightShaftVS"/>
            <state name="fragment" value="LightShaftPS"/>

            <state name="cullmode" value="front" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="LightScatterCopy">
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