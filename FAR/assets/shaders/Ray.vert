#version 460 core

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 color;

layout(location = 1) uniform mat4 Viewing;
layout(location = 2) uniform mat4 Projection;

layout(location = 0) out vec4 vert_color;

void main()
{
  vec4 pos = Projection * Viewing * vertex_position;
  gl_Position = pos;
  vert_color = color;
}