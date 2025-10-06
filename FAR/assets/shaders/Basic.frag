#version 460 core
//#extension GL_ARB_bindless_texture : require

//layout(binding = 0) buffer TextureHandles {
//    sampler2D textures[];
//};


//layout(binding = 0) uniform sampler2DArray textures;

layout(location = 6) uniform sampler2D textures[16];


//layout(location = 0) uniform sampler2DArray textures;


//layout(location = 0) uniform sampler2DArray tex;
layout(location = 4) uniform vec4 color;
layout(location = 5) uniform bool textured;

in vec4 frag_uv;
in vec4 vert_color;

out vec4 frag_color;

void main()
{

    if (textured)
    {
   //frag_color = texture(textures[int(frag_uv.z)], frag_uv.xy);

    frag_color = texture(textures[int(frag_uv.z)], frag_uv.xy);
   }
   else
    { 
    //frag_color = color;
    //frag_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);

    frag_color = vert_color;
    }

    //frag_color = vert_color;

}