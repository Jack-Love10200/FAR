#version 460 core
layout(location = 6) uniform sampler2D textures[16];
layout(location = 4) uniform vec4 color;
layout(location = 5) uniform bool textured;

in vec4 frag_uv;
in vec4 vert_color;

out vec4 frag_color;

void main()
{
    if (textured)
    {
      frag_color = texture(textures[int(frag_uv.z)], frag_uv.xy);
    }
    else
    { 
      frag_color = vert_color;
    }
}