cbuffer PER_BATCH:register(b0)
{
  float3 SkyDome_NightMoonTexGenUp;
  float4 vfViewPos;
  float3 SkyDome_NightMoonTexGenRight;
  float4 SkyDome_NightMoonDirSize;
  float3 vfSunDir;
  //miscCamFront;
  float3 vfColGradDelta;
  float4 vfRampParams;
  float4 vfColGradRadial;
  float4 vfParams;
  float4 vfColGradParams;
  float3 vfColGradBase;
};
cbuffer STATIC_INSTANCE:register(b2)
{
  float3x4 ObjWorldMatrix:packoffset(c0);
};
cbuffer PER_FRAME:register(b3)
{
  float4 g_VS_WorldViewPos:packoffset(c6);
  float4 g_VS_NearFarClipDist:packoffset(c9);
  float4x4 g_VS_ViewProjZeroMatr:packoffset(c10);
};
cbuffer PER_MATERIAL:register(b4)
{
  float3 __0bendDetailFrequency__1bendDetailLeafAmplitude__2bendDetailBranchAmplitude__3:packoffset(c2);
  float4 __0AnimFrequency__1AnimAmplitudeWav0__2AnimPhase__3AnimAmplitudeWav2:packoffset(c3);
};
float4x4 GetInstanceMatrix(bool bRelativeToCam=true)
{
  float4x4 Mat=float4x4(float4(1,0,0,0),float4(0,1,0,0),float4(0,0,1,0),float4(0,0,0,1));
  Mat[0]=ObjWorldMatrix[0];
  Mat[1]=ObjWorldMatrix[1];
  Mat[2]=ObjWorldMatrix[2];
  if(bRelativeToCam)
  {
    Mat[0].w-=g_VS_WorldViewPos.x;
    Mat[1].w-=g_VS_WorldViewPos.y;
    Mat[2].w-=g_VS_WorldViewPos.z;
  }
  return Mat;
}
float4 _pos_HPos(float4x4 VPMatrix,float4x4 InstMatrix,float4 InPos)
{
  float4 vWorldPos=mul(InstMatrix,InPos);
  return mul(VPMatrix,vWorldPos);
}
float4 _pos_Common(float4 InPos)
{
  float4x4 InstMatrix=GetInstanceMatrix(true);
  return _pos_HPos(g_VS_ViewProjZeroMatr,InstMatrix,InPos);
}
half ComputeVolumetricFogInternal(in float3 cameraToWorldPos)
{
  half fogInt=1.0h;
  float t=vfParams.x*cameraToWorldPos.z;
  if(abs(t)>0.01f)
  {
    fogInt*=(exp(t)-1.0f)/t;
  }
  const half l=length(cameraToWorldPos);
  const half u=l*half(vfParams.y);
  fogInt*=u;
  half f=saturate(exp2(-fogInt));
  half r=saturate(l*vfRampParams.x+vfRampParams.y);
  r=r*(2-r);
  r=r*vfRampParams.z+vfRampParams.w;
  f=(1-f)*r;
  return max(1-f,half(vfParams.w));
}
half4 GetVolumetricFogColorInternal(in float3 worldPos,in float3 cameraToWorldPos,in float radialFogShadowInfluence,in float ambientFogShadowInfluence)
{
  const half fog=ComputeVolumetricFogInternal(cameraToWorldPos);
  half h=saturate(worldPos.z*vfColGradParams.x+vfColGradParams.y);
  h=h*(2-h);
  half3 fogColor=(vfColGradBase+h*vfColGradDelta)*ambientFogShadowInfluence;
  const half l=saturate(length(cameraToWorldPos)*vfColGradRadial.w);
  const half radialLobe=pow(l,vfColGradParams.w);
  const half radialSize=exp2(dot(normalize(cameraToWorldPos),vfSunDir)*-vfColGradParams.z+vfColGradParams.z);
  fogColor+=radialLobe*radialSize*vfColGradRadial.xyz*radialFogShadowInfluence;
  return half4(fogColor,fog);
}
half4 GetVolumetricFogColorInternal(in float3 worldPos)
{
  vfViewPos.xyz=g_VS_WorldViewPos;
  float3 camToWorld=worldPos-vfViewPos.xyz;
  return GetVolumetricFogColorInternal(worldPos,camToWorld,1,1);
}
half4 GetVolumetricFogColor(in float3 worldPos,in float3 cameraToWorldPos,in float radialFogShadowInfluence,in float ambientFogShadowInfluence)
{
  return GetVolumetricFogColorInternal(worldPos,cameraToWorldPos,radialFogShadowInfluence,ambientFogShadowInfluence);
}
half4 GetVolumetricFogColor(in float3 worldPos,in float3 cameraToWorldPos)
{
  return GetVolumetricFogColorInternal(worldPos,cameraToWorldPos,1,1);
}
half4 GetVolumetricFogColor(in float3 worldPos)
{
  return GetVolumetricFogColorInternal(worldPos);
}
struct a2v
{
  float4 Position:POSITION;
  float2 baseTC:TEXCOORD0;
};
struct v2f_hdr
{
  float4 Position:POSITION;
  float4 packedTC:TEXCOORD0;
  float3 skyDir:TEXCOORD1;
};
v2f_hdr SkyHDRVS(a2v IN)
{
  v2f_hdr OUT=(v2f_hdr)0;
  float4 vPos=IN.Position;
  OUT.Position=_pos_Common(vPos);
  OUT.Position.z=OUT.Position.w;
  float2 baseTC=IN.baseTC;
  float2 moonTC=float2(dot(SkyDome_NightMoonTexGenRight,vPos),dot(SkyDome_NightMoonTexGenUp,vPos))*SkyDome_NightMoonDirSize.w+0.5;
  OUT.packedTC=float4(baseTC,0.0,1.0);
  OUT.skyDir=vPos;
  return OUT;
}