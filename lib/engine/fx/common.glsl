<?xml version="1.0"?>
<effect version="1270" language="glsl">
    <shader name="vertex">
        <![CDATA[
            #version 330 core
            #define GLSL_3 1

            #if GLSL_3 == 1
                #define float2 vec2
                #define float3 vec3
                #define float4 vec4
                #define int2 ivec2
                #define int3 ivec3
                #define int4 ivec4
                #define float3x3 mat3
                #define float4x3 mat4x3
                #define float4x4 mat4

                #define lerp(a, b, t) mix(a, b, t)
                #define saturate(a) clamp(a, 0.0, 1.0)
                #define mad(a, b, c) (a * b + c)
                #define mul(m, v) (m * v)

                layout(location = 0) in float4 glsl_Position;
                layout(location = 1) in float4 glsl_Normal;
                layout(location = 2) in float4 glsl_Texcoord;
                layout(location = 3) in float4 glsl_Diffuse;
                layout(location = 4) in float4 glsl_Specular;
                layout(location = 5) in float4 glsl_Weight;
                layout(location = 6) in float4 glsl_Tangent;
                layout(location = 7) in float4 glsl_Bitangent;
            #endif
        ]]>
    </shader>
    <shader name="fragment">
        <![CDATA[
            #version 330
            #define GLSL_3 1

            #if GLSL_3 == 1
                #define texture2d sampler2D
                #define sampleCoord(tex, coord) texture(tex, coord)
                #define sampleCoordOffset(tex, coord, offset) textureOffset(tex, coord, offset)
                #define sampleCoordLod(tex, coord, lod) textureLod(tex, coord, lod)
                #define sampleCoordOffsetLod(tex, coord, offset, lod) textureLodOffset(tex, coord, lod, offset)
                #define lerp(a, b, t) mix(a, b, t)
                #define saturate(a) clamp(a, 0.0, 1.0)
                #define mad(a, b, c) (a * b + c)
                #define mul(m, v) (m * v)

                #define float2 vec2
                #define float3 vec3
                #define float4 vec4
                #define int2 ivec2
                #define int3 ivec3
                #define int4 ivec4
                #define float3x3 mat3
                #define float4x3 mat4x3
                #define float4x4 mat4

                layout(location = 0) out vec4 glsl_FragColor0;
                layout(location = 1) out vec4 glsl_FragColor1;
                layout(location = 2) out vec4 glsl_FragColor2;
                layout(location = 3) out vec4 glsl_FragColor3;
                //layout(origin_upper_left) in vec4 gl_FragCoord;
            #endif

            float RGB2sRGB(float x)
            {
                if (x <= 0.04045f)
                    return x / 12.92f;
                else
                    return pow((x+0.055f)/(1+0.055f), 2.4f);
            }

            float sRGB2RGB(float x)
            {
                if (x <= 0.00031308)
                    return 12.92*x;
                else
                    return 1.055*pow(x,(1/2.4) ) - 0.055;
            }

            float3 RGB2sRGB(float3 c)
            {
                return float3(RGB2sRGB(c.x),RGB2sRGB(c.y),RGB2sRGB(c.z));
            }

            float4 RGBA2sRGB(float4 c)
            {
                return float4(RGB2sRGB(c.x),RGB2sRGB(c.y),RGB2sRGB(c.z), c.a);
            }

            float3 sRGB2RGB(float3 c)
            {
                return float3(sRGB2RGB(c.x),sRGB2RGB(c.y),sRGB2RGB(c.z));
            }

            float3 RGB2YCbCr(float3 rgb)
            {
                // RGB2YCbCr
                // float Y = 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
                // float Cb = 0.5 + (-0.168 * rgb.r - 0.331 * rgb.g + 0.5 * rgb.b);
                // float Cr = 0.5 + (0.5 * rgb.r - 0.418 * rgb.g - 0.081 * rgb.b);
                // return float3(Y, Cb, Cr);
                const float4x4 RGBToYCbCr = float4x4(
                    float4(0.299, -0.168,  0.5,   0.0),
                    float4(0.587, -0.331, -0.418, 0.0),
                    float4(0.114,  0.5,   -0.081, 0.0),
                    float4(0.0,    0.5,    0.5,   0.0) );
                float4 YCbCr = mul(RGBToYCbCr, float4(rgb, 1.0));
                return YCbCr.rgb;
            }

            float3 YCbCr2RGB(float3 YCbCr)
            {
                // YCbCr2RGB
                // float R = YCbCr.r + 1.402 * (YCbCr.b - 0.5);
                // float G = YCbCr.r - 0.344 * (YCbCr.g - 0.5) - 0.714 * (YCbCr.b - 0.5);
                // float B = YCbCr.r + 1.772 * (YCbCr.g - 0.5);
                // return float3(R, G, B);
                const float4x4 YCbCrToRGB = float4x4(
                    float4(1.0,    1.0,    1.0,   0.0),
                    float4(0.0,   -0.344,  1.772, 0.0),
                    float4(1.402, -0.714,  0.0,   0.0),
                    float4(-0.701, 0.529, -0.886, 0.0));
                float4 rgb = mul(YCbCrToRGB, float4(YCbCr, 1.0));
                return rgb.rgb;
            }

            float3 YCbCrMulYCbCr(float3 YCbCr1, float3 YCbCr2)
            {
                float Y = YCbCr1.r * YCbCr2.r;
                float Cb = (YCbCr1.g + YCbCr2.g) * 0.5;
                float Cr = (YCbCr1.b + YCbCr2.b) * 0.5;
                return float3(Y, Cb, Cr);
            }

            float3 YCbCrBlendYCbCr(float3 YCbCr1, float3 YCbCr2)
            {
                float Y = (YCbCr1.r + YCbCr2.r);
                float Cb = (YCbCr1.g + YCbCr2.g) * 0.5;
                float Cr = (YCbCr1.b + YCbCr2.b) * 0.5;
                return float3(Y, Cb, Cr);
            }

            float3 rgb2hsv(float3 rgb)
            {
                float minValue = min(min(rgb.r, rgb.g), rgb.b);
                float maxValue = max(max(rgb.r, rgb.g), rgb.b);
                float d = maxValue - minValue;

                float3 hsv = float3(0.0);
                hsv.b = maxValue;
                if (d != 0)
                {
                    hsv.g = d / maxValue;

                    float3 delrgb = (((float3(maxValue) - rgb) / 6.0) + d / 2.0) / d;
                    if      (maxValue == rgb.r) { hsv.r = delrgb.b - delrgb.g; }
                    else if (maxValue == rgb.g) { hsv.r = 1.0 / 3.0 + delrgb.r - delrgb.b; }
                    else if (maxValue == rgb.b) { hsv.r = 2.0 / 3.0 + delrgb.g - delrgb.r; }

                    if (hsv.r < 0.0) { hsv.r += 1.0; }
                    if (hsv.r > 1.0) { hsv.r -= 1.0; }
                }
                return hsv;
            }

            float3 rgb2xyz(float3 rgb)
            {
                const float3x3 m = float3x3 (
                    0.5141364, 0.3238786,  0.16036376,
                    0.265068,  0.67023428, 0.06409157,
                    0.0241188, 0.1228178,  0.84442666 );
                return mul(m, rgb);
            }

            float3 xyz2Yxy(float3 xyz)
            {
                float w = xyz.r + xyz.g + xyz.b;
                if (w > 0.0)
                {
                    float3 Yxy;
                    Yxy.r = xyz.g;
                    Yxy.g = xyz.r / w;
                    Yxy.b = xyz.g / w;
                    return Yxy;
                }
                else
                {
                    return float3(0.0);
                }
            }

            float3 Yxy2xyz(float3 Yxy)
            {
                float3 xyz;
                xyz.g = Yxy.r;
                if (Yxy.b > 0.0)
                {
                    xyz.r = Yxy.r * Yxy.g / Yxy.b;
                    xyz.b = Yxy.r * (1 - Yxy.g - Yxy.b) / Yxy.b;
                }
                else
                {
                    xyz.rb = float2(0.0);
                }
                return xyz;
            }

            float3 xyz2rgb(float3 xyz)
            {
                const float3x3 m  = float3x3(
                    2.5651, -1.1665, -0.3986,
                    -1.0217,  1.9777,  0.0439,
                    0.0753, -0.2543,  1.1892 );
                return mul(m, xyz);
            }

            float3 hsv2rgb(float3 hsv)
            {
                float h = hsv.r;
                float s = hsv.g;
                float v = hsv.b;

                float3 rgb = float3(v);
                if (hsv.g != 0.0)
                {
                    float h_i = floor(6 * h);
                    float f = 6 * h - h_i;

                    float p = v * (1.0 - s);
                    float q = v * (1.0 - f * s);
                    float t = v * (1.0 - (1.0 - f) * s);

                    if      (h_i == 0) { rgb = float3(v, t, p); }
                    else if (h_i == 1) { rgb = float3(q, v, p); }
                    else if (h_i == 2) { rgb = float3(p, v, t); }
                    else if (h_i == 3) { rgb = float3(p, q, v); }
                    else if (h_i == 4) { rgb = float3(t, p, v); }
                    else               { rgb = float3(v, p, q); }
                }
                return rgb;
            }

            float2 packDiffuse(float3 diffuse)
            {
                float3 YCbCr = RGB2YCbCr(diffuse.rgb);
                int2 crd = int2(gl_FragCoord.xy);
                bool pattern = (mod(crd.x,2.0)==mod(crd.y,2.0));
                return pattern ? YCbCr.rg : YCbCr.rb;
            }

            float3 unpackDiffuse(sampler2D mrt0, float2 coord)
            {
                int2 crd = int2(gl_FragCoord.xy);
                bool pattern = (mod(crd.x,2.0)==mod(crd.y,2.0));
                float3 YCbCr = sampleCoord(mrt0, coord).rgb;
                YCbCr.b = sampleCoordOffset(mrt0, coord, int2(1, 0)).g;
                YCbCr.rgb = (pattern) ? YCbCr.rgb : YCbCr.rbg;
                return YCbCr2RGB(YCbCr);
            }

            float3 packNormal(float3 normal)
            {
                float p = sqrt(-normal.z * 8 + 8);
                float2 enc = normal.xy / p + 0.5f;
                float2 enc255 = enc * 255;
                float2 residual = floor(fract(enc255) * 16);
                return float3(floor(enc255), residual.x * 16 + residual.y) / 255;
            }

            float3 unpackNormal(float3 enc)
            {
                float nz = floor(enc.z * 255) / 16;
                enc.xy += float2(floor(nz) / 16, fract(nz)) / 255;
                float2 fenc = enc.xy * 4 - 2;
                float f = dot(fenc, fenc);
                float g = sqrt(1 - f / 4);
                float3 normal;
                normal.xy = fenc * g;
                normal.z = f * 0.5 - 1;
                return normal;
            }

            float luminance(float3 rgb)
            {
                const float3 lumfact = float3(0.2126f, 0.7152f, 0.0722f);
                return dot(rgb, lumfact);
            }

            float4 StoreGBufferRT0(float4 diffuse, float3 specular)
            {
                return float4(diffuse.rgb, luminance(specular));
            }

            float4 StoreGBufferRT0(float4 diffuse, float specular)
            {
                return float4(diffuse.rgb, specular);
            }

            float4 StoreGBufferRT1(float3 n, float shininess)
            {
                return float4(packNormal(n), shininess);
            }

            float3 restoreRGB(sampler2D mrt0, float2 coord)
            {
                return sampleCoord(mrt0, coord).rgb;
            }

            float restoreSpecular(float4 mrt0)
            {
                return mrt0.a;
            }

            float restoreAlpha(float4 mrt0)
            {
                return mrt0.b;
            }

            float3 restoreNormal(float4 mrt1)
            {
                return unpackNormal(mrt1.xyz);
            }

            float restoreShininess(float4 mrt1)
            {
                return mrt1.w;
            }

            float3 restorePosition(sampler2D texDepth, mat4 matInverse, float2 coord)
            {
                float depth  = sampleCoord(texDepth, coord.xy).r;
                float4 result = matInverse * float4(coord * 2.0 - 1.0, depth, 1.0);
                result /= result.w;
                return result.xyz;
            }

            float3 restorePosition(sampler2D texDepthLinear, float4 projInfo, float2 coord)
            {
                float depth = sampleCoord(texDepthLinear, coord).r;
                return float3(projInfo.xy * coord + projInfo.zw, 1.0) * depth;
            }

            float4 sampleSphere(sampler2D source, float3 normal)
            {
                const float invPI = 1 / 3.1415926;
                float2 coord = float2(atan(-normal.x, normal.z) * invPI * 0.5 + 0.5, acos(normal.y) * invPI);
                return sampleCoord(source, coord);
            }
        ]]>
    </shader>
</effect>