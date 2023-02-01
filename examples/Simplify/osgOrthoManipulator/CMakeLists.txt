add_executable(osgOrthoManipulator osgOrthoManipulator.h osgOrthoManipulator.cpp main.cpp)
# Link to OpenSceneGraph libs
TARGET_LINK_LIBRARIES(osgOrthoManipulator ${OPENSCENEGRAPH_LIBRARIES} )

set_target_properties(${targetname} PROPERTIES VS_DEBUGGER_ENVIRONMENT "${MY_PATH}")