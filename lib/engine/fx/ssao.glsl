<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="projScale" type="float"/>
    <parameter name="projInfo" type="float4"/>
    <parameter name="clipInfo" type="float4"/>
    <parameter name="radius" type="float"/>
    <parameter name="radius2" type="float"/>
    <parameter name="bias" type="float"/>
    <parameter name="texSize" type="float2"/>
    <parameter name="intensity" type="float"/>
    <parameter name="blurFactor" type="float" />
    <parameter name="blurSharpness" type="float" />
    <parameter name="blurRadius" type="int" />
    <parameter name="blurDirection" type="float2"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texNormal" semantic="NormalMap" />
    <parameter name="texAO" type="sampler2D" />
    <parameter name="texSource" type="sampler2D"/>
    <parameter name="matView" semantic="matView" />
    <parameter name="matProject" semantic="matProject"/>
    <shader name="vertex">
        <![CDATA[
            out vec2 coord;
            out vec4 position;

            void postprocess()
            {
                coord = glsl_Texcoord.xy;
                position = vec4(coord.xy * vec2( 2.0f, -2.0f ) + vec2( -1.0f, 1.0f) , 0.0f, 1.0f );
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            #define NUM_SAMPLE 12

            in vec4 position;
            in vec2 coord;

            uniform float radius;
            uniform float radius2;
            uniform float projScale;
            uniform float bias;
            uniform float intensity;
            uniform vec4 projInfo;
            uniform vec4 clipInfo;
            uniform vec2 texSize;

            uniform int blurRadius;
            uniform float blurFactor;
            uniform float blurSharpness;
            uniform vec2 blurDirection;

            uniform mat4 matView;
            uniform mat4 matProject;

            uniform sampler2D texDepth;
            uniform sampler2D texNormal;
            uniform sampler2D texAO;
            uniform sampler2D texSource;

            float linearizeDepth(vec2 uv)
            {
                float d = textureLod(texDepth, uv, 0).r * 2.0 - 1.0;
                return clipInfo.x / (clipInfo.z - clipInfo.y * d);
            }

            vec3 samplePosition(vec2 uv)
            {
                float d = linearizeDepth(uv);
                return vec3(projInfo.xy * uv + projInfo.zw, 1.0) * d;
            }

            vec3 sampleNormal(vec2 uv)
            {
                vec4 normal = textureLod(texNormal, uv, 0);
                return mat3(matView) * restoreNormal(normal);
            }

            vec2 tapLocation(float sampleNumber, float spinAngle, float radius)
            {
                float alpha = (sampleNumber + 0.5) * (1.0 / NUM_SAMPLE);
                float angle = spinAngle + alpha * 6.28 * 7;
                return vec2(cos(angle), sin(angle)) * alpha * radius;
            }

            float computeAO(vec3 P, vec3 N, vec3 Q, float depth)
            {
                vec3 v = Q - P;

                float vv = dot(v, v);
                float vn = dot(v, N);

                const float epsilon = 0.01;
                return max((vn - depth * bias), 0) / (vv + epsilon);
            }

            void aoPS()
            {
                vec3 viewPosition = samplePosition(coord);
                vec3 viewNormal = sampleNormal(coord);

                float sampleDepth = linearizeDepth(coord);
                float sampleAngle = fract(pow(coord.x, coord.y) * 43758.5453) * texSize.x;
                float sampleRadius = projScale / viewPosition.z;
                float sampleAmbient = 0.0;
                float sampleWeight = 1.0 / NUM_SAMPLE;

                for (int tapIndex = 0; tapIndex < NUM_SAMPLE; ++tapIndex)
                {
                    vec2 sampleOffset = tapLocation(tapIndex, sampleAngle, sampleRadius);
                    vec2 sampleCoord = coord + sampleOffset;
                    vec3 samplePosition = samplePosition(sampleCoord);

                    sampleAmbient +=  computeAO(viewPosition, viewNormal, samplePosition, sampleDepth);
                }

                glsl_FragColor0 = vec4(max(0.0, 1 - sampleAmbient * intensity * sampleWeight));
            }

            float bilateralfilter(vec2 coord, float r, float center_d)
            {
                float d = linearizeDepth(coord);
                float ddiff = (d - center_d) * blurSharpness;
                return exp2(-r * r * blurFactor - ddiff * ddiff);
            }

            void blurPS()
            {
                float center_d = linearizeDepth(coord);

                float total_c = 0;
                float total_w = 0;

                for (int r = -blurRadius; r <= blurRadius; r++)
                {
                    vec2 offset = coord + blurDirection * r;

                    float bilateralWeight = bilateralfilter(offset, r, center_d);

                    total_c += textureLod(texSource, offset, 0).r * bilateralWeight;
                    total_w += bilateralWeight;
                }

                glsl_FragColor0 = vec4(total_c / total_w);
            }

            void copyPS()
            {
                glsl_FragColor0 = textureLod(texAO, coord, 0).rrrr;
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ao">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="aoPS"/>

            <state name="cullmode" value="front"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blur">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="blurPS"/>

            <state name="cullmode" value="front"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="copy">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="copyPS"/>

            <state name="cullmode" value="front"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="zero" />
            <state name="blenddst" value="srcalpha" />
        </pass>
    </technique>
</effect>