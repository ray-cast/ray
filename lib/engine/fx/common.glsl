<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <shader name="vertex">
        <![CDATA[
            #version 330 core

            layout(location = 0) in vec4 glsl_Position;
            layout(location = 1) in vec4 glsl_Normal;
            layout(location = 2) in vec4 glsl_Texcoord;
            layout(location = 3) in vec4 glsl_Diffuse;
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            #version 330 core

            layout(location = 0) out vec4 glsl_FragColor0;
            layout(location = 1) out vec4 glsl_FragColor1;
            layout(location = 2) out vec4 glsl_FragColor2;
            layout(location = 3) out vec4 glsl_FragColor3;
            //layout(origin_upper_left) in vec4 gl_FragCoord;

            #define float2 vec2
            #define float3 vec3
            #define float4 vec4
            #define int2 ivec2
            #define int3 ivec3
            #define int4 ivec4
            #define float4x4 mat4x4

            vec4 StoreGBufferRT0(vec3 diffuse, vec3 specular)
            {
                return vec4(diffuse, dot(specular, vec3(0.2126f, 0.7152f, 0.0722f)));
            }

            vec4 StoreGBufferRT0(vec4 diffuse, vec3 specular)
            {
                //dot(specular, vec3(0.2126f, 0.7152f, 0.0722f))
                return vec4(diffuse.rgb, diffuse.a);
            }

            vec4 StoreGBufferRT1(vec3 normal, float shininess)
            {
                float p = sqrt(-normal.z * 8 + 8);
                vec2 enc = normal.xy / p + 0.5f;
                vec2 enc255 = enc * 255;
                vec2 residual = floor(fract(enc255) * 16);
                return vec4(vec3(floor(enc255), residual.x * 16 + residual.y) / 255, shininess / 255);
            }

            vec3 restoreNormal(vec4 texture)
            {
                float nz = floor(texture.z * 255) / 16;
                texture.xy += vec2(floor(nz) / 16, fract(nz)) / 255;
                vec2 fenc = texture.xy * 4 - 2;
                float f = dot(fenc, fenc);
                float g = sqrt(1 - f / 4);
                vec3 normal;
                normal.xy = fenc * g;
                normal.z = f / 2 - 1;
                return normal;
            }

            vec3 restoreDiffuse(vec4 mrt0)
            {
                return mrt0.xyz;
            }

            float restoreSpecular(vec4 mrt0)
            {
                return mrt0.w;
            }

            float restoreShininess(vec4 mrt1)
            {
                return mrt1.w * 256.0f;
            }
        ]]>
    </shader>
</effect>