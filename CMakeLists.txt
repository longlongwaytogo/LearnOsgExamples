CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(LearnOsgExamples)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)


option(use_gl3 "use gl3" ON)

cmake_policy(SET CMP0074 NEW)

set(OpenSceneGraph_ROOT "" CACHE PATH "osg root path")
set(OpenSceneGraph_BINARY_ROOT "" CACHE PATH "osg binary root path")

set( include_dir "")
set(MY_PATH ""  CACHE FILEPATH "")
set(MY_CUSTOM_PATH ${OpenSceneGraph_BINARY_ROOT}/bin)
set(QT_BIN_DIR "${QT_DIR}/../../../bin/")
message(STATUS ${QT_BIN_DIR})
set(MY_PATH "PATH=%PATH%" ${MY_CUSTOM_PATH} ${QT_BIN_DIR})

#EXECUTABLE_OUTPUT_PATH
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#LIBRARY_OUTPUT_PATH
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

FIND_PACKAGE(OpensceneGraph  COMPONENTS osg osgDB osgUtil osgViewer osgText osgGA osgManipulator osgVolume)

include(${CMAKE_CURRENT_SOURCE_DIR}/findCustomOSG.cmake)
IF(NOT OPENSCENEGRAPH_FOUND)
	MESSAGE("------------------is openscengraph found:${OPENSCENEGRAPH_FOUND}")
ELSE()
	MESSAGE("------------------is openscengraph found:${OPENSCENEGRAPH_FOUND}")
ENDIF()
 # find_package(osgViewer)
 # find_package(osgText) 	
 # find_package(osgUtil)
 # find_package(osgDB)
 # find_package(osgGA)
 # find_package(osgManipulator)
 # find_package(osgVolume)
 # find_package(openThreads)
 # find_package(osgShadow)
 # FIND_PACKAGE(osgFX)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include") 
INCLUDE_DIRECTORIES(${OPENSCENEGRAPH_INCLUDE_DIRS} ${OpenSceneGraph_BINARY_ROOT}/include)

Set(OSGLIBPATH ""  CACHE FILEPATH "")
LINK_DIRECTORIES(
	${OPENSCENEGRAPH_LIBRARIES_DIRS}
	${OSGDB_LIBRARIES_DIRS}
	${OSGTEXT_LIBRARIES_DIRS}
	${OSGUTIL_LIBRARIES_DIRS}
	${OSGGA_LIBRARIES_DIRS}
	${OSGMANIPULATOR_LIBRARIES_DIRS}
	${OSG_LIBRARIES_DIRS}
	${OPENTHREADS_LIBRARIES_DIRS}
	${OSGSHADOW_LIBRARIES_DIRS}
	${OSGFX_LIBRARIES_DIRS}
	${OSGLIBPATH}
	 ) 

MESSAGE("${OSGLIBPATH}")
MESSAGE(-----------------------------------)
MESSAGE("${OPENSCENEGRAPH_INCLUDE_DIRS}")
MESSAGE(-----------------------------------)
MESSAGE("${OPENSCENEGRAPH_LIBRARIES}")
MESSAGE(-----------------------------------)
# MESSAGE( ${OPENTHREADS_LIBRARIES_DIRS} )
MESSAGE("LINK_LIB")
MESSAGE("${OPENSCENEGRAPH_LIBRARIES}")

LINK_LIBRARIES(${OPENSCENEGRAPH_LIBRARIES})

 

link_libraries(
${OSG_LIBRARIES}
${OSGVIEWER_LIBRARIES}
${OSGDB_LIBRARIES} 
${OSGUTIL_LIBRARIES}
${OSGTEXT_LIBRARIES}
${OSGGA_LIBRARIES}
${OSGMANIPULATOR_LIBRARIES}
${OPENTHREADS_LIBRARIES}
${OSGSHADOW_LIBRARIES}
${OSGFX_LIBRARIES}
 )
 


# examples
ADD_SUBDIRECTORY(examples)

 
 