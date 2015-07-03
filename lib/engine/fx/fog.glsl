<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="matViewProjectInverse" semantic="matViewProjectInverse"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="eyePosition" semantic="CameraPosition"/>
    <parameter name="fogFalloff" type="float"/>
    <parameter name="fogDensity" type="float"/>
    <parameter name="fogColor" type="float3"/>
    <shader name="vertex">
        <![CDATA[
            out vec4 coord;

            void FogVS()
            {
                coord = glsl_Texcoord;
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            uniform sampler2D texDepth;

            uniform vec3 eyePosition;

            uniform mat4 matViewProjectInverse;

            uniform float fogFalloff;
            uniform float fogDensity;
            uniform vec3 fogColor;

            in vec4 coord;

            vec4 samplePosition(vec2 coord)
            {
                float depth  = textureLod(texDepth, coord.xy, 0).r;
                vec4 result = matViewProjectInverse * vec4(coord * 2.0 - 1.0, depth, 1.0);
                result /= result.w;
                return result;
            }

            float fogFactorLinear(float dist, float start, float end)
            {
                float fog = (end - dist) / (end - start);
                return clamp(fog, 0, 1);
            }

            float fogFactorLinear(float dist, float density)
            {
                float fog = 1 - density * dist;
                return clamp(fog, 0, 1);
            }

            float fogFactorExp(float dist, float density)
            {
                float fog = 1 - exp(-density * dist);
                return clamp(fog, 0, 1);
            }

            float fogFactorExp2(float dist, float density)
            {
                const float LOG2 = -1.442695;
                float dc = density * dist;
                float fog = 1 - exp2(dc * dc * LOG2);
                return clamp(fog, 0, 1);
            }

            float fogFactor(float dist, vec3 rayOrigin, vec3 rayDirection)
            {
                float fogAmount = exp((-rayOrigin.y*fogFalloff*fogDensity)) * (1.0-exp(-dist*rayDirection.y*fogFalloff*fogDensity)) / rayDirection.y * fogFalloff;
                return clamp(fogAmount, 0, 1);
            }

            void FogPS()
            {
                vec3 P = samplePosition(coord.xy).rgb;
                float dist = distance(eyePosition, P);
                //glsl_FragColor0 = vec4(fogColor, fogFactor(dist, eyePosition, normalize(P)));
                glsl_FragColor0 = vec4(fogColor, fogFactorExp(dist, fogFalloff * fogDensity));
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="fog">
            <state name="vertex" value="FogVS"/>
            <state name="fragment" value="FogPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="cullmode" value="front" />

            <state name="blend" value="true"/>
            <state name="blendsrc" value="srcalpha"/>
            <state name="blenddst" value="invsrcalpha"/>
        </pass>
    </technique>
</effect>