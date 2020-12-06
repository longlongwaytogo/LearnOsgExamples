add_executable(osgOrthoManipulator osgOrthoManipulator.h osgOrthoManipulator.cpp main.cpp)
# Link to OpenSceneGraph libs
TARGET_LINK_LIBRARIES(osgOrthoManipulator ${OPENSCENEGRAPH_LIBRARIES} )
