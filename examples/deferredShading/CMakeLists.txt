add_executable(deferredshading main.cpp deferredShading.cpp)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)
TARGET_LINK_LIBRARIES(deferredshading 
	 debug EffectCompositor${CMAKE_DEBUG_POSTFIX}          optimized EffectCompositor)
	

# https://bitbucket.org/kornerr/osg-deferred-shading/downloads/

# FBO MRT ªÊ÷∆‘≠¿Ì
# https://blog.csdn.net/u010462297/article/details/40343105
