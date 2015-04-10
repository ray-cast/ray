<?xml version='1.0'?>
<effect version="1270" language="glsl">
    <technique name="shadow">
        <pass name="p0">
            <state name="zenable" value="true"/>
            <state name="zwriteenable" value="true"/>
            <shader type="vertex">
                <![CDATA[
                    #version 330 core

                    layout(location = 0) in vec4 position;

                    uniform mat4 matModel;
                    uniform mat4 matViewProject;

                    out Vertex
                    {
                        vec4 position;
                    } vertex;

                    void main()
                    {
                        gl_Position = vertex.position = matViewProject * matModel * position;
                    }
                ]]>
            </shader>
            <shader type="fragment">
                <![CDATA[
                    #version 330 core

                    layout(location = 0) out float oColor0;

                    uniform float shadow_bias;
                    uniform float shadow_factor;

                    in Vertex
                    {
                        vec4 position;
                    } vertex;

                    void main()
                    {
                        //exponential shadow mapping
                        oColor0 = exp(shadow_bias + shadow_factor * vertex.position.z);
                    }
                ]]>
            </shader>
        </pass>
    </technique>
    <technique name="blurh">
        <pass name="blurh">
            <shader type="vertex">
                <![CDATA[
                    #version 330 core

                    layout(location = 0) in vec4 position;
                    layout(location = 7) in vec2 texcoord;

                    out vec2 UV;

                    void main()
                    {
                        UV = texcoord;
                        gl_Position = position;
                    }
                ]]>
            </shader>
            <shader type="fragment">
                <![CDATA[
                    #version 330 core

                    #extension GL_EXT_texture_array : enable

                    layout(location = 0) out float oColor0;

                    uniform int index;
                    uniform sampler2DArray decal;

                    in vec2 UV;

                    const float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
                    const float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

                    void main()
                    {
                        vec2 size = vec2(textureSize(decal, 0));

                        oColor0 = texture2DArray(decal, vec3(UV, index)).r * weight[0];

                        for (int i = 1; i < 3; ++i)
                        {
                            oColor0 += texture2DArray(decal, vec3(UV + vec2(offset[i], 0.0) / size.x, index)).r * weight[i];
                            oColor0 += texture2DArray(decal, vec3(UV - vec2(offset[i], 0.0) / size.x, index)).r * weight[i];
                        }
                    }
              ]]>
            </shader>
        </pass>
    </technique>
    <technique name="blurv">
        <pass name="blurv">
            <shader type="vertex">
                <![CDATA[
                    #version 330 core

                    layout(location = 0) in vec4 position;
                    layout(location = 7) in vec2 texcoord;

                    out vec2 UV;

                    void main()
                    {
                        UV = texcoord;
                        gl_Position = position;
                    }
                ]]>
            </shader>
            <shader type="fragment">
                <![CDATA[
                    #version 330 core

                    #extension GL_EXT_texture_array : enable

                    layout(location = 0) out float oColor0;

                    uniform int index;
                    uniform sampler2DArray decal;

                    in vec2 UV;

                    const float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
                    const float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

                    void main()
                    {
                        vec2 size = vec2(textureSize(decal, 0));

                        oColor0 = texture2DArray(decal, vec3(UV, index)).r * weight[0];

                        for (int i = 1; i < 3; ++i)
                        {
                            oColor0 += texture2DArray(decal, vec3(UV + vec2(0.0, offset[i]) / size.y, index)).r * weight[i];
                            oColor0 += texture2DArray(decal, vec3(UV - vec2(0.0, offset[i]) / size.y, index)).r * weight[i];
                        }
                    }
                ]]>
            </shader>
        </pass>
    </technique>
    <technique name="gbuffer">
        <pass name="p0">
            <state name="zenable" value="true"/>
            <state name="zwriteenable" value="true"/>
            <shader type="vertex">
                <![CDATA[
                    #version 330
                    precision mediump float;

                    layout(location = 0) in vec4 position;
                    layout(location = 1) in vec4 norm;
                    layout(location = 4) in vec2 uv;

                    uniform mat4 matModel;
                    uniform mat4 matNormal;

                    layout(std140) uniform camera
                    {
                        mat4 ViewProject;
                    } matrix;

                    out vec3 normal;
                    out vec2 texcoord;

                    void main()
                    {
                        normal = (matNormal * norm).xyz;
                        texcoord = uv;
                        gl_Position = matrix.ViewProject * matModel * position;
                    }
                ]]>
            </shader>
            <shader type="fragment">
                <![CDATA[
                    #version 330
                    precision mediump float;

                    layout(location = 0) out vec4 oColor0;
                    layout(location = 1) out vec4 oColor1;

                    in vec3 normal;
                    in vec2 texcoord;

                    uniform vec3 diffuse;
                    uniform vec3 ambient;
                    uniform vec3 emissive;

                    uniform float opacity;
                    uniform float specular;
                    uniform float shininess;
                    uniform float reflectivity;
                    uniform float refraction_ratio;

                    uniform bool has_difftex_mapping;
                    uniform bool has_normals_mapping;

                    uniform sampler2D diffuse_mapping;
                    uniform sampler2D normals_mapping;
                    uniform sampler2D normals_fitting;

                    void main()
                    {
                        if (has_normals_mapping) {
                            oColor0.rgb = texture2D(normals_mapping, texcoord).rgb;
                        } else {
                            oColor0.rgb = normalize(normal);
                        }

                        oColor0.rgb = gl_FrontFacing ? oColor0.rgb : -oColor0.rgb;
                        oColor0.rgb = oColor0.rgb * 0.5 + 0.5;
                        oColor0.a   = shininess / 256.0;

                        if (has_difftex_mapping) {
                            oColor1.rgb = texture2D(diffuse_mapping, texcoord).rgb;
                        } else {
                            oColor1.rgb = diffuse;
                        }

                        oColor1.a = floor(specular * 100) + opacity / 10;
                    }

                    vec3 computeFittingCoord(vec3 normal)
                    {
                        vec3 N = abs(normal);

                        float M = max(N.z, max(N.x, N.y));

                        vec2 texcoord = N.z < M ? (N.y < M ? N.yz : N.xz) : N.xy;

                        texcoord = texcoord.x < texcoord.y ? texcoord.yx : texcoord.xy;
                        texcoord.y /= texcoord.x;

                        return vec3(texcoord, 1 / M);
                    }

                    vec3 restoreNormal(vec3 normal)
                    {
                        vec3 UVM = computeFittingCoord(normal);

                        normal.xyz *= UVM.z;

                        normal.xyz *= texture2D(normals_fitting, UVM.xy).a;

                        normal.xyz = normal.xyz * 0.5 + 0.5;

                        return normal;
                    }
                ]]>
            </shader>
        </pass>
    </technique>
    <technique name="lighting">
        <pass name="p0">
            <state name="blendenable" value="true"/>
            <state name="blendsrcfactor" value="one"/>
            <state name="blenddestfactor" value="one"/>
            <state name="zwriteenable" value="true"/>
            <state name="zenable" value="false"/>
            <shader type="vertex">
                <![CDATA[
                    #version 330 core

                    layout(location = 0) in vec4 position;

                    uniform mat4 matModel;
                    uniform mat4 matViewProject;

                    out vec4 g_position;

                    void main()
                    {
                        gl_Position = g_position = matViewProject * matModel * position;
                    }
                ]]>
            </shader>
            <shader type="fragment">
                <![CDATA[
                    #version 330 core

                    #extension GL_EXT_texture_array : enable

                    layout(location = 0) out vec4 oColor0;

                    in vec4 g_position;

                    uniform bool has_shadow;

                    uniform float light_range;
                    uniform float light_intensity_inverse;
                    uniform float angle_outer_cone;
                    uniform float exponent;
                    uniform float constant_attenuation;
                    uniform float linear_attenuation;
                    uniform float quadratic_attenuation;
                    uniform float shadow_bias;
                    uniform float shadow_factor;

                    uniform vec3 eye_position;
                    uniform vec3 light_color;
                    uniform vec3 light_direction;
                    uniform vec3 light_position;

                    uniform mat4 matShadowCubeMatrix[6];
                    uniform mat4 matViewProjectInverse;

                    uniform sampler2D depth_mapping;
                    uniform sampler2D normal_mapping;
                    uniform sampler2DArray shadow_mapping;

                    float length2(float n)
                    {
                        return dot(n, n);
                    }

                    float saturate(float n)
                    {
                        return clamp(n, 0.0, 1.0);
                    }

                    vec4 unproject(vec3 p)
                    {
                        // proeject position
                        vec4 result = matViewProjectInverse * vec4(p, 1.0);
                        result /= result.w;
                        return result;
                    }

                    float diffuse_lighting(vec3 N, vec3 L)
                    {
                        // 双面光照
                        // return abs(dot(N, L));
                        return max(dot(N, L), 0);
                    }

                    float specular_lighting(vec3 N, vec3 H, float shininess)
                    {
                        return pow(max(0, dot(N, H)), shininess);
                    }

                    vec4 directional_light(vec3 world, vec3 N, float shininess)
                    {
                        vec3 L = normalize(light_direction);
                        vec3 V = normalize(eye_position - world);
                        vec3 H = normalize(L + V);

                        float diffuse = diffuse_lighting(N, L);

                        float specular = specular_lighting(N, H, shininess);

                        return vec4(light_color, specular) * diffuse;
                    }

                    vec4 point_light(vec3 world, vec3 N, float shininess)
                    {
                        vec3 L = normalize(light_position - world);
                        vec3 V = normalize(eye_position - world);
                        vec3 H = normalize(L + V);

                        float d2 = length2(L);

                        float attenuation = 1 - light_intensity_inverse - light_intensity_inverse / light_range * d2;

                        float diiffuse = diffuse_lighting(N, L);

                        float specular = specular_lighting(N, H, shininess);

                        return vec4(light_color, specular) * diiffuse * attenuation;
                    }

                    vec4 spot_light(vec3 world, vec3 N, float shininess)
                    {
                        vec3 L = normalize(light_position - world);
                        vec3 V = normalize(eye_position - world);
                        vec3 H = normalize(L + V);

                        float d = length(L);

                        float attenuation = 1 / (constant_attenuation - linear_attenuation*d + quadratic_attenuation*d*d);

                        float dotSpot = dot(-L, normalize(light_direction));
                        float cosCutoff = cos(angle_outer_cone * 3.1415926 / 180.0);

                        if (dotSpot >= cosCutoff)
                            attenuation *= pow(dotSpot, exponent);

                        float diiffuse = diffuse_lighting(N, L);

                        float specular = specular_lighting(N, H, shininess);

                        return vec4(light_color, specular) * diiffuse * attenuation;
                    }

                    float shadow_lighting(int index, vec3 world)
                    {
                        vec4 proj = matShadowCubeMatrix[index] * vec4(world, 1.0);
                        proj.xy /= proj.w;
                        proj.xy = proj.xy * 0.5 + 0.5;
                        proj.w = index;

                        float occluder = texture2DArray(shadow_mapping, proj.xyw).r;

                        float receiver = exp(shadow_bias -shadow_factor * proj.z);
                        float shadow = occluder * receiver;

                        return saturate(shadow);
                    }

                    float shadow_cube_lighting(vec3 world)
                    {
                        vec3 L = light_position - world;

                        float axis[6];
                        axis[0] =  L.x;
                        axis[1] = -L.x;
                        axis[2] =  L.y;
                        axis[3] = -L.y;
                        axis[4] =  L.z;
                        axis[5] = -L.z;

                        int index = 0;
                        for (int i = 1; i < 6; i++)
                        {
                            if (axis[i] > axis[index])
                            {
                                index = i;
                            }
                        }

                        return shadow_lighting(index, world);
                    }

                    vec4 lighting(vec3 world, vec3 N, float shininess)
                    {
                        if (angle_outer_cone == 180.0)
                        {
                            return directional_light(world, N, shininess);
                        }
                        else if (angle_outer_cone > 180.0)
                        {
                            return point_light(world, N, shininess);
                        }
                        else
                        {
                            return spot_light(world, N, shininess);
                        }
                    }

                    float shadowing(vec3 world)
                    {
                        if (angle_outer_cone <= 180.0)
                        {
                            return shadow_lighting(0, world);
                        }
                        else
                        {
                            return shadow_cube_lighting(world);
                        }
                    }

                    void main()
                    {
                        // normalize proeject position
                        vec3 position;
                        position.xy = g_position.xy / g_position.w;

                        // conver to scene position
                        vec2 UV = position.xy * 0.5 + 0.5;

                        position.z  = texture2D(depth_mapping, UV).r * 2.0 - 1.0;
                        if (position.z == 1.0)
                        {
                            discard;
                        }

                        // vertex position
                        vec3 P = unproject(position).xyz;

                        // xyz = normal; w = shininess
                        vec4 NS = texture2D(normal_mapping, UV);

                        // normal
                        vec3 N = NS.xyz * 2.0 - 1.0;

                        // shininess
                        float shininess = NS.w * 256.0;

                        vec4 color = lighting(P, N, shininess);

                        if (has_shadow)
                        {
                            color *= shadowing(P);
                        }

                        oColor0 = color;
                    }
                ]]>
            </shader>
        </pass>
    </technique>
</effect>