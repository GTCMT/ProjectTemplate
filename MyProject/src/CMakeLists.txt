cmake_minimum_required (VERSION 2.8)
project (MyProject)

#######################################################################################################
## includes
# where to look for the default stuff...
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../cmake.modules")

# platform detection
include (platform)

# compiler flags
include (compiler-options)

# output directories
include (output)

# oslibs
include (oslibs)

# helper functions
include(functions)


#######################################################################################################
## project specific stuff

# set directories
set (PROJECT_DIRECTORIES)
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}) 
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}CL)       

# set includes
set (PROJECT_INCLUDES)
list (APPEND PROJECT_INCLUDES ${CMAKE_SOURCE_DIR}/../incl)       
list (APPEND PROJECT_INCLUDES ${CMAKE_BINARY_DIR}/incl)       

# set dependencies
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/../../SharedSources/AudioFileIO)       
list (APPEND PROJECT_INCLUDES ${CMAKE_SOURCE_DIR}/../../SharedSources/incl)       
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/../../SharedSources/TestCL)       

# add directories
add_directories ()


# set version number variable
set (${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" CACHE INTERNAL "")
message(STATUS ${PROJECT_NAME} Version: ${${PROJECT_NAME}_VERSION})
message(STATUS "")

#######################################################################################################
## tests
enable_testing()

# add tests here
#add_test()
add_cppcheck_test ()
