set(targetname ModelManipulator)
add_executable(${targetname} main.cpp)
# Link to OpenSceneGraph libs
TARGET_LINK_LIBRARIES(${targetname} ${OPENSCENEGRAPH_LIBRARIES} )
SET_PROPERTY(TARGET ${targetname} PROPERTY FOLDER "ModelOper")
set_target_properties(${targetname} PROPERTIES VS_DEBUGGER_ENVIRONMENT "${MY_PATH}")