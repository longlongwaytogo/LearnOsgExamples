// cbuffer PER_BATCH:register(b0)
// {
  // float3 SkyDome_NightMoonTexGenUp;
  // float4 vfViewPos;
  // float3 SkyDome_NightMoonTexGenRight;
  // float4 SkyDome_NightMoonDirSize;
  // float3 vfSunDir;
  // //miscCamFront;
  // float3 vfColGradDelta;
  // float4 vfRampParams;
  // float4 vfColGradRadial;
  // float4 vfParams;
  // float4 vfColGradParams;
  // float3 vfColGradBase;
// };
// cbuffer STATIC_INSTANCE:register(b2)
// {
  // float3x4 ObjWorldMatrix:packoffset(c0);
// };
// cbuffer PER_FRAME:register(b3)
// {
  // float4 g_VS_WorldViewPos:packoffset(c6);
  // float4 g_VS_NearFarClipDist:packoffset(c9);
  // float4x4 g_VS_ViewProjZeroMatr:packoffset(c10);
// };
// cbuffer PER_MATERIAL:register(b4)
// {
  // float3 __0bendDetailFrequency__1bendDetailLeafAmplitude__2bendDetailBranchAmplitude__3:packoffset(c2);
  // float4 __0AnimFrequency__1AnimAmplitudeWav0__2AnimPhase__3AnimAmplitudeWav2:packoffset(c3);
// };
// float4x4 GetInstanceMatrix(bool bRelativeToCam=true)
// {
  // float4x4 Mat=float4x4(float4(1,0,0,0),float4(0,1,0,0),float4(0,0,1,0),float4(0,0,0,1));
  // Mat[0]=ObjWorldMatrix[0];
  // Mat[1]=ObjWorldMatrix[1];
  // Mat[2]=ObjWorldMatrix[2];
  // if(bRelativeToCam)
  // {
    // Mat[0].w-=g_VS_WorldViewPos.x;
    // Mat[1].w-=g_VS_WorldViewPos.y;
    // Mat[2].w-=g_VS_WorldViewPos.z;
  // }
  // return Mat;
// }
// float4 _pos_HPos(float4x4 VPMatrix,float4x4 InstMatrix,float4 InPos)
// {
  // float4 vWorldPos=mul(InstMatrix,InPos);
  // return mul(VPMatrix,vWorldPos);
// }

// float4 _pos_Common(float4 InPos)
// { 
  // float4x4 InstMatrix=GetInstanceMatrix(true);
  // return _pos_HPos(g_VS_ViewProjZeroMatr,InstMatrix,InPos);
// }




// struct a2v
// {
  // float4 Position:POSITION;
  // float2 baseTC:TEXCOORD0;
// };
 
 
 
 

// float3 SkyDome_NightMoonTexGenUp;
  // float4 vfViewPos;
  // float3 SkyDome_NightMoonTexGenRight;
  // float4 SkyDome_NightMoonDirSize;
  // float3 vfSunDir;
  // //miscCamFront;
  // float3 vfColGradDelta;
  // float4 vfRampParams;
  // float4 vfColGradRadial;
  // float4 vfParams;
  // float4 vfColGradParams;
  // float3 vfColGradBase;
  
  // uniform 
 vec3 SkyDome_NightMoonTexGenRight;
 vec3 SkyDome_NightMoonTexGenUp;
 vec4 SkyDome_NightMoonDirSize = vec4(0.35355,0.61237,0.70711,13);
// varying vec4 vs_Position;
 varying vec4 vs_packedTC;
 varying vec3 vs_skyDir;

void main()
{
  vec4 vPos= gl_Vertex;
  vec4 vs_Position= gl_ProjectionMatrix * gl_ModelViewMatrix * vPos;//_pos_Common(vPos); // 将坐标变换到投影空间mvp
 // vs_Position.z=vs_Position.w;
  gl_Position = vs_Position;
  vec2 baseTC= gl_MultiTexCoord0.xy;
  vec2 moonTC=vec2(dot(SkyDome_NightMoonTexGenRight,vPos),dot(SkyDome_NightMoonTexGenUp,vPos))*SkyDome_NightMoonDirSize.w+0.5;
  vs_packedTC=vec4(baseTC,moonTC.yx);
  vs_skyDir=vPos;
 // return OUT;
}