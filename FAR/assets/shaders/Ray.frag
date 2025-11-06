#version 460 core

layout(location = 0) in vec4 vert_color;
out vec4 frag_color;

void main()
{
  frag_color = vert_color;
}