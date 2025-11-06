///
/// @file   basic.vert
/// @brief  Vertex shader for basic rendering with skinning support.
/// @author Jack Love
/// @date   11.10.2025
///

#version 460 core
layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 uv;
layout(location = 2) in vec4 weights;
layout(location = 3) in ivec4 bones;

layout(location = 1) uniform mat4 Modeling;
layout(location = 2) uniform mat4 Viewing;
layout(location = 3) uniform mat4 Projection;

layout(location = 50) uniform bool useSkinning;

layout(location = 51) uniform mat4 BoneTransforms[200];

out vec4 frag_uv;
out vec4 vert_color;

void main()
{   
    bool hasValidSkinning = useSkinning;
    vec4 totalPosition = vec4(0.0f);

    //ensure weights add up to one
    vec4 weightsNorm = normalize(weights);

    if (weightsNorm.x + weightsNorm.y + weightsNorm.z + weightsNorm.w == 0.0f)
        weightsNorm = vec4(1.0f, 0.0f, 0.0f, 0.0f);

    //skip skinning if no bones are assigned
    if (bones.x == -1)
        hasValidSkinning = false;



    //compute skinning matrix
    mat4 skinMat =
    BoneTransforms[bones.x] * weightsNorm.x +
    BoneTransforms[bones.y] * weightsNorm.y +
    BoneTransforms[bones.z] * weightsNorm.z +
    BoneTransforms[bones.w] * weightsNorm.w;

    //apply skinning to the vertex position
    if (hasValidSkinning)
    totalPosition = skinMat * vec4(vertex_position.xyz, 1.0f);
    else
    totalPosition = vec4(vertex_position.xyz, 1.0f);

    //outputs to the fragment shader
    gl_Position = Projection * Viewing * Modeling * totalPosition;
    vert_color = vec4(uv.xy, 1.0f, 1.0f);

//    vec4 outcol = vec4(0.0f);
//
//    if (bones.x != -1)
//        outcol.x = 1.0f;
//
//        if (bones.y != -1)
//        outcol.y = 1.0f;
//
//        if (bones.z != -1)
//        outcol.z = 1.0f;
//
//        if (bones.w != -1)
//        outcol.w = 0.50f;
//        else
//        outcol.w = 1.0f;

    //vert_color = outcol;
    frag_uv = uv;
}