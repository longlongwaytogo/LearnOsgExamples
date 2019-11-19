add_executable(osgCloud render_points.cpp CommonFunctions.cpp CommonFunctions)
# Link to OpenSceneGraph libs
TARGET_LINK_LIBRARIES(osgCloud ${OPENSCENEGRAPH_LIBRARIES} )
