#version 330 core

in float fragDepth;

out vec2 moments;

void main()
{
    float d = fragDepth;
    moments = vec2(d, d*d);
}