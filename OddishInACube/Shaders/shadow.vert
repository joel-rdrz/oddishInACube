#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 mvp;

out float fragDepth;

void main()
{
    gl_Position = mvp * vec4(pos, 1);
    fragDepth = gl_Position.z / gl_Position.w * 0.5 + 0.5 + 0.02;
}