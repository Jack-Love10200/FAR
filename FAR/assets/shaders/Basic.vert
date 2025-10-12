#version 460 core
layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 uv;
layout(location = 2) in vec4 weights;
layout(location = 3) in ivec4 bones;

layout(location = 1) uniform mat4 Modeling;
layout(location = 2) uniform mat4 Viewing;
layout(location = 3) uniform mat4 Projection;

layout(location = 200) uniform bool useSkinning;

layout(location = 50) uniform mat4 BoneTransforms[100];

out vec4 frag_uv;
out vec4 vert_color;

void main()
{
    vec4 totalPosition = vec4(0.0f);

    //compute skinning matrix
    mat4 skinMat =
    BoneTransforms[bones.x] * weights.x +
    BoneTransforms[bones.y] * weights.y +
    BoneTransforms[bones.z] * weights.z +
    BoneTransforms[bones.w] * weights.w;

    //apply skinning to the vertex position
    if (useSkinning)
    totalPosition = skinMat * vec4(vertex_position.xyz, 1.0f);
    else
    totalPosition = vec4(vertex_position.xyz, 1.0f);

    //outputs to the fragment shader
    gl_Position = Projection * Viewing * Modeling * totalPosition;
    vert_color = vec4(uv.xy, 1.0f, 1.0f);
    frag_uv = uv;
}