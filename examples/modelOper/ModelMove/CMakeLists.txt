add_executable(ModelMove main.cpp)
# Link to OpenSceneGraph libs
TARGET_LINK_LIBRARIES(ModelMove ${OPENSCENEGRAPH_LIBRARIES} )
SET_PROPERTY(TARGET ModelMove PROPERTY FOLDER "ModelOper")
set_target_properties(${targetname} PROPERTIES VS_DEBUGGER_ENVIRONMENT "${MY_PATH}")