set(targetName MouseZoom)
add_executable(${targetName} main.cpp OrbitManipulatorEx.h OrbitManipulatorEx.cpp)
# Link to OpenSceneGraph libs
TARGET_LINK_LIBRARIES(${targetName} ${OPENSCENEGRAPH_LIBRARIES} )

SET_PROPERTY(TARGET ${targetName} PROPERTY FOLDER "ModelOper")

set_target_properties(${targetName} PROPERTIES VS_DEBUGGER_ENVIRONMENT "${MY_PATH}")
