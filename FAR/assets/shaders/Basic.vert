#version 460 core
layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 uv;

layout(location = 1) uniform mat4 Modeling;
layout(location = 2) uniform mat4 Viewing;
layout(location = 3) uniform mat4 Projection;
out vec3 frag_uv;
void main()
{
   gl_Position = Projection * Viewing * Modeling * vertex_position;
   frag_uv = uv;
}