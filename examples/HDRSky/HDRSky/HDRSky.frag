// uniform vec4 SkyDome_NightMoonOuterCoronaColorScale;
// uniform vec4 SkyDome_NightMoonDirSize;
// uniform vec2 SkyDome_NightSkyZenithColShift;
// uniform vec3 SkyDome_PartialMieInScatteringConst;
// uniform vec4 SkyDome_NightMoonInnerCoronaColorScale;
// uniform vec3 SkyDome_NightSkyColBase;
// uniform vec3 SkyDome_PhaseFunctionConstants;
// uniform vec3 SkyDome_PartialRayleighInScatteringConst;
// uniform vec3 SkyDome_SunDirection;
// uniform vec3 SkyDome_NightSkyColDelta;
// uniform vec3 SkyDome_NightMoonColor;
//uniform vec4 g_PS_SunLightDir;
 vec4 SkyDome_NightMoonOuterCoronaColorScale = vec4(0,0,0,2001.0);
 vec4 SkyDome_NightMoonDirSize= vec4(0.35355,0.61237,0.70711,13.00);
 vec2 SkyDome_NightSkyZenithColShift = vec2(1.17647,0.11765);
 vec3 SkyDome_PartialMieInScatteringConst = vec3(0.00494,0.00494,0.00494);
 vec4 SkyDome_NightMoonInnerCoronaColorScale =vec4(0.0,0.0,0.0,2001.0);
 vec3 SkyDome_NightSkyColBase = vec3(0,0,0);
 vec3 SkyDome_PhaseFunctionConstants = vec3(42.61117,42.61332,0.0);
 vec3 SkyDome_PartialRayleighInScatteringConst = vec3(0.00067,0.00167,0.00251);
 vec3 SkyDome_SunDirection = vec3(0,0,1);
 vec3 SkyDome_NightSkyColDelta = vec3(0,0,0);
 vec3 SkyDome_NightMoonColor = vec3(0,0,0);
 vec4 g_PS_SunLightDir = vec4(0,0,1,1);
 
uniform sampler2D skyDomeSamplerMie;
uniform sampler2D skyDomeSamplerRayleigh;
uniform sampler2D moonSampler;
 
 //varying vec4 vs_Position;
 varying vec4 vs_packedTC;
 varying vec3 vs_skyDir;

void main()
{
  vec2 baseTC=vs_packedTC.xy;
  vec2 moonTC=vs_packedTC.wz;
  vec3 skyDir=normalize(vs_skyDir);
  vec4 Color=vec4(0,0,0,1);
  vec4 ColorMie=texture2D(skyDomeSamplerMie,baseTC.xy);
  vec4 ColorRayleigh=texture2D(skyDomeSamplerRayleigh,baseTC.xy);
  float miePart_g_2=SkyDome_PhaseFunctionConstants.x;
  float miePart_g2_1=SkyDome_PhaseFunctionConstants.y;
  float cosine=-dot(SkyDome_SunDirection,skyDir);
  float cosine2=cosine*cosine;
  float miePhase=(1.0+cosine2)*pow(miePart_g2_1+miePart_g_2*cosine,-1.5);
  float rayleighPhase=0.75*(1.0+cosine2);
  Color.xyz=ColorMie.xyz*SkyDome_PartialMieInScatteringConst*miePhase+ColorRayleigh.xyz*SkyDome_PartialRayleighInScatteringConst*rayleighPhase;
  float gr=clamp(vs_skyDir.z*SkyDome_NightSkyZenithColShift.x+SkyDome_NightSkyZenithColShift.y,0.0,1.0);
  gr*=2-gr;
  Color.xyz+=SkyDome_NightSkyColBase+SkyDome_NightSkyColDelta*gr;
  vec4 moonAlbedo=texture2D(moonSampler,moonTC.xy);
  Color.xyz+=SkyDome_NightMoonColor.xyz*moonAlbedo.xyz*moonAlbedo.a;
  float m=1-dot(skyDir,SkyDome_NightMoonDirSize.xyz);
  Color.xyz+=SkyDome_NightMoonInnerCoronaColorScale.xyz*(1.0/(1.05+m*SkyDome_NightMoonInnerCoronaColorScale.w));
  Color.xyz+=SkyDome_NightMoonOuterCoronaColorScale.xyz*(1.0/(1.05+m*SkyDome_NightMoonOuterCoronaColorScale.w));
  Color.xyz*=g_PS_SunLightDir.w;
  Color.xyz=min(Color.xyz,vec3(16384.0));
  gl_FragColor = Color;
 // gl_FragColor = vec4(1.0,0.0,0.0,1.0);
}