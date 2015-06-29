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
        ]]>
    </shader>
</effect>