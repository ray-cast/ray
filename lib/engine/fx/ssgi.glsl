<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <include name="sys:fx/common.glsl"/>
    <parameter name="projScale" type="float"/>
    <parameter name="projInfo" type="float4"/>
    <parameter name="clipInfo" type="float4"/>
    <parameter name="radius" type="float"/>
    <parameter name="radius2" type="float"/>
    <parameter name="bias" type="float"/>
    <parameter name="intensityDivR6" type="float"/>
    <parameter name="blurFactor" type="float" />
    <parameter name="blurSharpness" type="float" />
    <parameter name="blurRadius" type="int" />
    <parameter name="blurDirection" type="float2"/>
    <parameter name="texDepth" semantic="DepthMap" />
    <parameter name="texNormal" semantic="NormalMap" />
    <parameter name='texColor' semantic="ColorMap" />
    <parameter name="texAO" type="sampler2D" />
    <parameter name="texSource" type="sampler2D"/>
    <parameter name="matViewInverseTranspose" semantic="matViewInverseTranspose" />
    <parameter name="matProjectInverse" semantic="matProjectInverse"/>
    <shader name="vertex">
        <![CDATA[
            out vec2 coord;

            void postprocess()
            {
                coord = glsl_Texcoord.xy;
                gl_Position = glsl_Position;
            }
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            #define NUM_SPIRAL_TURNS 7
            #define NUM_SAMPLE 6

            varying vec4 position;
            varying vec2 coord;

            uniform float radius;
            uniform float radius2;
            uniform float projScale;
            uniform float bias;
            uniform float intensityDivR6;
            uniform vec4 projInfo;
            uniform vec4 clipInfo;

            uniform int blurRadius;
            uniform float blurFactor;
            uniform float blurSharpness;
            uniform vec2 blurDirection;

            uniform mat4 matViewInverseTranspose;

            uniform sampler2D texDepth;
            uniform sampler2D texNormal;
            uniform sampler2D texColor;
            uniform sampler2D texAO;
            uniform sampler2D texSource;

            float heaviside(float x)
            {
                return 0.5 * (1 + sign(x));
            }

            float linearizeDepth(vec2 uv)
            {
                float d = texture(texDepth, uv).r * 2.0 - 1.0;
                return clipInfo.x / (clipInfo.y * d - clipInfo.z);
            }

            float bilateralfilter(vec2 coord, float r, float center_d)
            {
                float d = linearizeDepth(coord);
                float ddiff = (d - center_d);
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

            float computeAO(vec3 c, vec3 n, vec3 q)
            {
                vec3 v = normalize(q - c);

                float vv = dot(v, v);
                float vn = dot(v, n);

                float f = max(radius2 - vv, 0);

                const float epsilon = 0.001;
                return heaviside(f) * max((vn - bias) / (vv + epsilon), 0);
            }

            void aoPS()
            {
                ivec2 ssC = ivec2(gl_FragCoord.xy);

                vec3 viewPosition = getPosition(coord);
                vec3 viewNormal = texture2D(texNormal, coord).rgb;

                float sampleRadius = projScale / viewPosition.z;
                float sampleSpinAngle = (3 * ssC.x ^ ssC.y + ssC.x * ssC.y) * 10;
                float sampleWeight = 1.0;

                vec3 sampleAmbient = vec3(0.0, 0.0, 0.0);

                for (int tapIndex = 0; tapIndex < NUM_SAMPLE; ++tapIndex)
                {
                    vec2 sampleOffset = tapLocation(tapIndex, sampleSpinAngle, sampleRadius);
                    vec2 sampleCoord = coord + sampleOffset;
                    vec3 samplePosition = getPosition(sampleCoord);
                    vec3 sampleDirection = samplePosition - viewPosition;
                    vec3 sampleColor = texture2D(texColor, sampleCoord).rgb;

                    float vv = dot(sampleDirection, sampleDirection);
                    if (vv <= radius2)
                    {
                        sampleAmbient += sampleColor * computeAO(viewPosition, viewNormal, samplePosition);
                        sampleWeight += 1.0;
                    }
                }

                glsl_FragColor0.rgb = sampleAmbient / sampleWeight;
            }

            void blurPS()
            {
                float center_d = linearizeDepth(coord);

                vec3 total_c = vec3(0,0,0);
                float total_w = 0;

                for (int r = -blurRadius; r <= blurRadius; r++)
                {
                    vec2 offset = coord + blurDirection * r;

                    float bilateralWeight = bilateralfilter(offset, r, center_d);

                    total_c += texture2D(texSource, offset).rgb * bilateralWeight;
                    total_w += bilateralWeight;
                }

                glsl_FragColor0.rgb = total_c / total_w;
            }

            void copyPS()
            {
                glsl_FragColor0 = texture2D(texAO, coord);
            }
        ]]>
    </shader>
    <technique name="postprocess">
        <pass name="ao">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="aoPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="blur">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="blurPS"/>

            <state name="depthtest" value="false"/>
            <state name="depthwrite" value="false"/>
        </pass>
        <pass name="copy">
            <state name="vertex" value="postprocess"/>
            <state name="fragment" value="copyPS"/>

            <state name="depthtest" value="true"/>
            <state name="depthwrite" value="false"/>

            <state name="blend" value="true"/>
            <state name="blendsrc" value="one" />
            <state name="blenddst" value="one" />
        </pass>
    </technique>
</effect>