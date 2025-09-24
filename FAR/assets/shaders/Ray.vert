#version 460 core

layout(location = 0) in vec4 vertex_position;

layout(location = 1) uniform mat4 Viewing;
layout(location = 2) uniform mat4 Projection;

void main()
{

  vec4 pos = Projection * Viewing * vertex_position;
  //pos.z = 20.0f;

   gl_Position = pos;
}