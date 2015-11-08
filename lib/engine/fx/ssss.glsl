<?xml version='1.0'?>
<effect language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="sssWidth" type="float" />
    <parameter name="strength" type="float"/>
    <parameter name="correction" type="float" />
    <parameter name="lightFarPlane" type="float" />
    <parameter name="lightDirection" type="float3" />
    <parameter name="texLight" semantic="DeferredLightMap"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texNormal" semantic="NormalMap" />
    <parameter name="texColor" type="sampler2D" />
    <parameter name="lightShadowMap" type="sampler2D" />
    <parameter name='lightShadowMatrix' type="mat4" />
    <parameter name="matViewProjectInverse" semantic="matViewProjectInverse"/>
    <shader name="vertex">
        <![CDATA[
            out vec4 coord;

            void postprocesss()
            {
                coord = glsl_Texcoord;
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
        #define SSSS_GLSL_3 1

        uniform sampler2D texColor;
        uniform sampler2D texDepth;
        uniform sampler2D texNormal;
        uniform sampler2D texLight;

        uniform float sssWidth;
        uniform float strength;
        uniform float correction;
        uniform float lightFarPlane;
        uniform float3 lightDirection;
        uniform float4 clipInfo;

        uniform mat4 lightShadowMatrix;
        uniform mat4 matViewProjectInverse;

        uniform sampler2D lightShadowMap;

        in vec4 coord;

        vec3 samplePosition(vec2 coord)
        {
            float depth  = sampleCoord(texDepth, coord).r;
            vec4 result = matViewProjectInverse * vec4(coord * 2.0 - 1.0, depth, 1.0);
            result /= result.w;
            return result.xyz;
        }

        vec3 sampleNormal(vec2 uv)
        {
            vec4 normal = sampleCoord(texNormal, uv);
            return restoreNormal(normal);
        }

        float4 SSSSTransmittance(float translucency, float sssWidth)
        {
            float3 normal = sampleNormal(coord.xy);
            float3 position = samplePosition(coord.xy);

            float4 shadowPosition = lightShadowMatrix * float4(position, 1.0);
            shadowPosition.xyz /= shadowPosition.w;
            shadowPosition.xyz = shadowPosition.xyz * 0.5 + 0.5;

            float scale = 8.25 * (1.0 - translucency) / sssWidth;
            float d1 = sampleCoord(lightShadowMap, shadowPosition.xy).r;
            float d2 = shadowPosition.z;
            float d = scale * abs(d2 - d1);
            float dd = -d * d;
            float3 profile = float3(0.233, 0.455, 0.649) * exp(dd / 0.0064) +
                             float3(0.1,   0.336, 0.344) * exp(dd / 0.0484) +
                             float3(0.118, 0.198, 0.0)   * exp(dd / 0.187)  +
                             float3(0.113, 0.007, 0.007) * exp(dd / 0.567)  +
                             float3(0.358, 0.004, 0.0)   * exp(dd / 1.99)   +
                             float3(0.078, 0.0,   0.0)   * exp(dd / 7.41);
            float3 diffuse = restoreRGB(texLight, coord.xy);
            float irradiance = max(0.3 + dot(lightDirection, -normal), 0.0);

            return float4(diffuse * profile * irradiance, 0.0);
        }

        float4 SSSSBlurPS(float2 texcoord, sampler2D colorTex, sampler2D depthTex, float strength, float2 dir)
        {
            float w[6] = float[](0.006,   0.061,   0.242,  0.242,  0.061, 0.006);
            float o[6] = float[](-1.0, -0.6667, -0.3333, 0.3333, 0.6667,   1.0);

            float4 colorM = sampleCoord(colorTex, texcoord);
            float depthM = sampleCoord(depthTex, texcoord).r;

            float2 finalStep = strength * dir / depthM;

            float4 colorBlurred = colorM;
            colorBlurred.rgb *= 0.382;

            for (int i = 0; i < 6; i++)
            {
                float2 offset = texcoord +  o[i] * finalStep;
                float3 color = sampleCoord(colorTex, offset).rgb;
                float depth = sampleCoord(depthTex, offset).r;
                float s = min(correction * abs(depthM - depth), 1.0);
                colorBlurred.rgb += w[i] * lerp(color, colorM.rgb, s);
            }

            return colorBlurred;
        }

        void TranslucencyPS()
        {
            glsl_FragColor0 = SSSSTransmittance(0.83, sssWidth);
        }

        void BlurX()
        {
            glsl_FragColor0 = SSSSBlurPS(coord.xy, texColor, texDepth, strength, float2(1.0, 0.0));
        }

        void BlurY()
        {
            glsl_FragColor0 = SSSSBlurPS(coord.xy, texColor, texDepth, strength, float2(0.0, 1.0));
        }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="translucency">
            <state name="vertex" value="postprocesss"/>
            <state name="fragment" value="TranslucencyPS"/>

            <state name="cullmode" value="front" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blenddst" value="one"/>
            <state name="blendsrc" value="one"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="notequal"/>
            <state name="stencilRef" value="0" />
        </pass>
        <pass name="blurX">
            <state name="vertex" value="postprocesss"/>
            <state name="fragment" value="BlurX"/>

            <state name="cullmode" value="front" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blurY">
            <state name="vertex" value="postprocesss"/>
            <state name="fragment" value="BlurY"/>

            <state name="cullmode" value="front" />

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="stencilTest" value="true"/>
            <state name="stencilFunc" value="notequal"/>
            <state name="stencilRef" value="0" />
        </pass>
    </technique>
</effect>