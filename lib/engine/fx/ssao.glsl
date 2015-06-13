<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <parameter name="projScale" type="float"/>
    <parameter name="projInfo" type="float4"/>
    <parameter name="clipInfo" type="float4"/>
    <parameter name="radius" type="float"/>
    <parameter name="bias" type="float"/>
    <parameter name="intensityDivR6" type="float"/>
    <parameter name="blurFactor" type="float" />
    <parameter name="blurSharpness" type="float" />
    <parameter name="blurRadius" type="int" />
    <parameter name="blurDirection" type="int2"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texNormal" semantic="NormalMap" />
    <parameter name="texAO" type="sampler2D" />
    <parameter name="texSource" type="sampler2D"/>
    <parameter name="matView" semantic="matView" />
    <parameter name="matProjectInverse" semantic="matProjectInverse"/>
    <shader>
        <![CDATA[
            #define NUM_SPIRAL_TURNS 7
            #define NUM_SAMPLE 10

            varying vec4 position;
            varying vec2 coord;

            uniform float radius;
            uniform float projScale;
            uniform float bias;
            uniform float intensityDivR6;
            uniform vec4 projInfo;
            uniform vec4 clipInfo;

            uniform int blurRadius;
            uniform float blurFactor;
            uniform float blurSharpness;
            uniform ivec2 blurDirection;

            uniform mat4 matView;

            uniform sampler2D texDepth;
            uniform sampler2D texNormal;
            uniform sampler2D texAO;
            uniform sampler2D texSource;

#if SHADER_API_VERTEX
            void mainVS()
            {
                coord = glsl_Texcoord;
                gl_Position = position = glsl_Position;
            }
#endif

            float heaviside(float x)
            {
                return 0.5 * (1 + sign(x));
            }

            float linearizeDepth(vec2 uv)
            {
                float d = texture2D(texDepth, uv).r * 2.0 - 1.0;
                return clipInfo.x / (clipInfo.y * d - clipInfo.z);
            }

            float linearizeDepth(ivec2 uv)
            {
                float d = texelFetch(texDepth, uv, 0).r;
                return clipInfo.x / (clipInfo.y * d - clipInfo.z);
            }

            float bilateralfilter(ivec2 uv, float r, float center_c, float center_d)
            {
                float d = linearizeDepth(uv);
                float ddiff = d - center_d;
                return exp(-r * r * blurFactor - ddiff * ddiff * blurSharpness);
            }

            vec3 getPosition(vec2 uv)
            {
                float d = linearizeDepth(uv);
                return vec3((projInfo.xy * uv + projInfo.zw) * d, d);
            }

            vec2 tapLocation(int sampleNumber, float spinAngle, float diskRadius)
            {
                float alpha = (sampleNumber + 0.5) * (1.0 / NUM_SAMPLE);
                float angle = alpha * (NUM_SPIRAL_TURNS * 6.28) + spinAngle;
                return vec2(cos(angle), sin(angle)) * alpha * diskRadius;
            }

            float computeAO(vec3 c, vec3 n, vec3 q, float radius2)
            {
                vec3 v = q - c;

                float vv = dot(v, v);
                float vn = dot(v, n);

                float f = max(radius2 - vv, 0);

                const float epsilon = 0.0001;
                return heaviside(f) * max((vn - bias) / (vv + epsilon), 0);
            }

            float sampleAO(int tapIndex, vec2 uv, vec3 c, vec3 n, float spinAngle, float diskRadius, float radius2)
            {
                vec2 offset = tapLocation(tapIndex, spinAngle, diskRadius);
                vec3 q = getPosition(uv + offset);
                return computeAO(c, n, q, radius2);
            }

#if SHADER_API_FRAGMENT
            void aoPS()
            {
                vec3 viewPosition = getPosition(coord);
                vec3 viewNormal = mat3(matView) * texture2D(texNormal, coord).rgb;

                ivec2 ssC = ivec2(gl_FragCoord.xy);
                float spinAngle = (3 * ssC.x ^ ssC.y + ssC.x * ssC.y) * 10;
                float diskRadius = projScale * radius / viewPosition.z;
                float radius2 = radius * radius;

                float sum = 0.0;
                for (int i = 0; i < NUM_SAMPLE; ++i)
                {
                    sum += sampleAO(i, coord, viewPosition, viewNormal, spinAngle, diskRadius, radius2);
                }

                float ao = max(0.0, 1 - sum / NUM_SAMPLE * intensityDivR6);

                glsl_FragColor0 = vec4(ao);
            }

            void blurPS()
            {
                ivec2 ssC = ivec2(gl_FragCoord.xy);

                float center_c = texelFetch(texSource, ssC, 0).r;
                float center_d = linearizeDepth(ssC);

                float sum = 0;
                float total = 0;

                for (int r = -blurRadius; r <= blurRadius; r++)
                {
                    ivec2 offset = ssC + blurDirection * r;

                    float bilateralWeight = bilateralfilter(offset, r, center_c, center_d);

                    sum += texelFetch(texSource, offset, 0).r * bilateralWeight;
                    total += bilateralWeight;
                }

                const float epsilon = 0.0001;
                glsl_FragColor0 = vec4(sum / (total + epsilon));
            }

            void copyPS()
            {
                glsl_FragColor0 = vec4(texture2D(texAO, coord).r);
            }
#endif
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ao">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="aoPS"/>
        </pass>
        <pass name="blur">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="blurPS"/>
        </pass>
        <pass name="copy">
            <state name="vertex" value="mainVS"/>
            <state name="fragment" value="copyPS"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="zero" />
            <state name="blenddst" value="srccol" />
        </pass>
    </technique>
</effect>