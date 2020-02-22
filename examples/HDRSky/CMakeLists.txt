

# INCLUDE_DIRECTORIES(${OPENSCENEGRAPH_INCLUDE_DIR})
# LINK_DIRECTORIES(${OPENSCENEGRAPH_LIB_DIR})

SET(EXAMPLE_FILES
    osgeffectcompositor.cpp
    SkyBox.cpp
    EffectAnalysisHelper.cpp
    EffectCompositor.cpp
    EffectCompositorXML.cpp
	HDRSky/HDRSky.h
	HDRSky/HDRSkyComm.h
	HDRSky/HDRSky.cpp
	HDRSky/SkyLightNishita.h
	HDRSky/SkyLightNishita.cpp
	HDRSky/SkyLightManager.h
	HDRSky/SkyLightManager.cpp
)


# # osgFXEffectCompositor library.
# ADD_LIBRARY(osgFXEffectCompositor SHARED EffectCompositor.cpp
                                         # EffectCompositorXML.cpp)
add_executable(HDRSky ${EXAMPLE_FILES})
TARGET_LINK_LIBRARIES(HDRSky
    debug osg${CMAKE_DEBUG_POSTFIX}          optimized osg
    debug osgAnimation${CMAKE_DEBUG_POSTFIX} optimized osgAnimation
    debug osgParticle${CMAKE_DEBUG_POSTFIX}  optimized osgParticle
    debug osgShadow${CMAKE_DEBUG_POSTFIX}    optimized osgShadow
    debug osgDB${CMAKE_DEBUG_POSTFIX}        optimized osgDB
    debug osgGA${CMAKE_DEBUG_POSTFIX}        optimized osgGA
    debug osgText${CMAKE_DEBUG_POSTFIX}      optimized osgText
    debug osgUtil${CMAKE_DEBUG_POSTFIX}      optimized osgUtil
    debug osgViewer${CMAKE_DEBUG_POSTFIX}    optimized osgViewer
    debug OpenThreads${CMAKE_DEBUG_POSTFIX}  optimized OpenThreads
)
    


	