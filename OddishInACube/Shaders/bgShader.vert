#version 330 core

layout(location = 0) in vec3 pos;

out vec3 dir; 

uniform mat4 mvp;

void main()
{
    
    dir = pos;

    gl_Position = mvp * vec4(pos, 1.0);

}