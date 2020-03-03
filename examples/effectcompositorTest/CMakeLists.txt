
# osgFXEffectCompositor library.


add_executable(osgEffectCompositorTest EffectAnalysisHelper.cpp osgeffectcompositor.cpp SkyBox.cpp)
target_link_libraries(osgEffectCompositorTest EffectCompositor)
 target_link_libraries(osgEffectCompositorTest debug osgShadowd   optimized osgShadow)