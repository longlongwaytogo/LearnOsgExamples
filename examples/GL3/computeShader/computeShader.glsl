
#version 430
#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

layout(binding = 0) uniform sampler2D depthTex; // 输入深度图
layout (rgba32f, binding =1) uniform image2D targetTex; //写入纹理 
layout (local_size_x = 1, local_size_y = 1,local_size_z = 1) in;
uniform float osg_FrameTime;
uniform mat4 scene_mvp;
uniform mat4 scene_mvpInverse;
uniform vec4 screenSize;

vec4 getNearestDepth()
{
	int w = int(screenSize.x);
	int h = int(screenSize.y);
	vec4 depthUV = vec4(0,0,1.0,1.0);
	for(int i = 0; i < w; ++i)
	{
		for(int j = 0; j < h; ++j)
		{
			vec2 uv = vec2(i*screenSize.z,j*screenSize.w);
			float depth = textureLod(depthTex,uv,0.0).r;
			if(depth < depthUV.z)
			{
				depthUV = vec4(uv,depth,1.0);
			}
		}
	}
	return depthUV;
}


void StoreWorldToBuffer(ivec2 storePos,vec4 worldPos)
{
	imageStore(targetTex,storePos,worldPos);
}

vec4 depthToWorldPos(vec4 depth)
{
	float d = depth.z;
	depth *=2.0;
	depth -=1.0;
	vec4 worldPos = scene_mvpInverse * depth;
	worldPos /= worldPos.w;
	worldPos.w = d;
	return worldPos;
}
void main()
{
	
//	
//	float coeffcient = 0.5*sin(float(gl_WorkGroupID.x + gl_WorkGroupID.y)*0.1 + osg_FrameTime);
//	coeffcient *= length(vec2(ivec2(gl_LocalInvocationID.xy) - ivec2(8)) / vec2(8.0));
//	imageStore(targetTex, storePos, vec4(1.0-coeffcient, 0.0, coeffcient, 0.0));
//
//	return;

	// save center worldPos
	float d = textureLod(depthTex,vec2(0.5,0.5),0.0).r;
	vec4 pos = vec4(0.5,0.5,d,1.0);
	vec4 worldPos = depthToWorldPos(pos);
	StoreWorldToBuffer(ivec2(0,0),worldPos);
	
	// save nearest worldPos
	vec4 nearestDepth = getNearestDepth();
	worldPos = depthToWorldPos(nearestDepth);
	StoreWorldToBuffer(ivec2(1,0),worldPos);
	 
}

//uniform float osg_FrameTime;
////layout(binding = 0) uniform sampler2D depthTex; // 输入深度图
//layout (r32f, binding =1) uniform image2D targetTex;
//layout (local_size_x = 16, local_size_y = 16) in;
//void main() {
//   ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
//   float coeffcient = 0.5*sin(float(gl_WorkGroupID.x + gl_WorkGroupID.y)*0.1 + osg_FrameTime);
//   coeffcient *= length(vec2(ivec2(gl_LocalInvocationID.xy) - ivec2(8)) / vec2(8.0));
//   imageStore(targetTex, storePos, vec4(1.0-coeffcient, 0.0, 0.0, 0.0));
//} 
// 

#endif //COMPUTESHADER_H