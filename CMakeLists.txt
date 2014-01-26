cmake_minimum_required (VERSION 2.8)
project (MyProject)

#######################################################################################################
## includes
# where to look for the default stuff...
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake.modules")

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
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/src/${PROJECT_NAME}) 
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/src/${PROJECT_NAME}CL)       
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/src/JuceLibraryCode)  
if (WITH_JUCE)
	list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/src/${PROJECT_NAME}Gui)  
endif (WITH_JUCE)

# set includes
set (PROJECT_INCLUDES)
list (APPEND PROJECT_INCLUDES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/incl)       
list (APPEND PROJECT_INCLUDES ${CMAKE_BINARY_DIR}/incl)       

# set dependencies
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/SharedSources/AudioFileIO)       
list (APPEND PROJECT_INCLUDES ${CMAKE_SOURCE_DIR}/SharedSources/incl)       
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/SharedSources/TestCL)       
list (APPEND PROJECT_DIRECTORIES ${CMAKE_SOURCE_DIR}/SharedSources/Dsp)       

# add directories
add_directories ()


# set version number variable
set (${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}" CACHE INTERNAL "")
message(STATUS ${PROJECT_NAME} Version: ${${PROJECT_NAME}_VERSION})
message(STATUS "")

#######################################################################################################
## tests

if (WITH_TESTS)
	enable_testing()
	
	# add tests here
	add_test(AudioIo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestCL AudioIo "${CMAKE_SOURCE_DIR}/Tests/testdata/")
	add_test(Fft ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestCL Fft)
	add_test(RingBuff ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestCL RingBuff)
	add_test(MyProject ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TestCL MyProject)
	add_cppcheck_test ()
endif (WITH_TESTS)

