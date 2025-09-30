#version 460 core
layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 uv;
layout(location = 2) in vec4 weights;
layout(location = 3) in ivec4 bones;

layout(location = 1) uniform mat4 Modeling;
layout(location = 2) uniform mat4 Viewing;
layout(location = 3) uniform mat4 Projection;

layout(location = 50) uniform mat4 BoneTransforms[100];

out vec3 frag_uv;
out vec4 vert_color;

void main()
{

    vec4 totalPosition = vec4(0.0f);
    
    bool validBoneFound = false;

        for (uint i = 0; i < 1 ; i++)
        {
            //if(bones[i] == -1) continue;

            //vec4 localPosition = BoneTransforms[bones[i]] * vec4(vertex_position.xyz,1.0f);
            //vec4 localPosition = BoneTransforms[bones[i]] * vertex_position;
            //totalPosition += localPosition * weights[i];
            //totalPosition += localPosition;


            validBoneFound = true;
        }
    
    if (!validBoneFound || totalPosition == vec4(0.0))
    {
		//totalPosition = vec4(vertex_position.xyz, 1.0);
		totalPosition = vertex_position;
	}


  mat4 skinMat =
    BoneTransforms[bones.x] * weights.x +
    BoneTransforms[bones.y] * weights.y +
    BoneTransforms[bones.z] * weights.z +
    BoneTransforms[bones.w] * weights.w;

    totalPosition = skinMat * vec4(vertex_position.xyz, 1.0f);

    gl_Position = Projection * Viewing * Modeling * totalPosition;

    //gl_Position = gl_Position / gl_Position.w;

    vert_color = vec4(weights[0], 1 - weights[1], weights[2], weights[3]);

    vert_color = vec4(gl_Position.xyz / gl_Position.w * 0.5 + 0.5, 1.0);

    vert_color = vec4(
    float(bones[0]) / 4,
    float(bones[1]) / 4,
    float(bones[2]) / 4,
    1.0);

    vert_color = vec4(uv.xy, 1.0f, 1.0f);

  frag_uv = uv.xyz;
}





//void main()
//{
//  float totalweight = 0.0f;
//  mat4 skinning = mat4(0.0);
//  for(int i = 0; i < 4; i++)
//  {
//    if(weights[i] > 0.0)
//    {
//      skinning += weights[i] * BoneTransforms[bones[i]];
//      totalweight += weights[i];
//    }
//  }
//
//  if (totalweight > 0.0)
//    skinning /= totalweight;
//  else
//    skinning = mat4(1.0);
//
//
//  mat4 skinnedModeling = Modeling * skinning;
//  //mat4 skinnedModeling = skinning * Modeling;
//
//  gl_Position = Projection * Viewing * skinnedModeling * vertex_position;
//  //gl_Position = Projection * Viewing * Modeling * vertex_position;
//
//  //gl_Position = BoneTransforms[bones[0]][3];
//
//  frag_uv = uv.xyz;
//}