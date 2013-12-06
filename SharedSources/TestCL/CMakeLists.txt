set (APP Test)

set (LIB MyProject)
set (TestLIB UnitTest++)
set (AudioLIB AudioFileIO)
set (SndLIB sndlib)

# with tests or not
option (WITH_TESTS "Use UnitTest++ for testing" ON)

# get library source files
if(WITH_TESTS)
	add_subdirectory(${CMAKE_SOURCE_DIR}/../../3rdPartyLibs/UnitTest++ "${CMAKE_CURRENT_BINARY_DIR}/UnitTest++")
	list (APPEND PROJECT_INCLUDES ${CMAKE_SOURCE_DIR}/../../3rdPartyLibs/UnitTest++)       

	## get all source files
	# this is the global main
	file(GLOB ${APP}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} Test.cpp)
	file(GLOB ${APP}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR} *.h;../incl/*.h;../../../incl/SharedSources/incl/*.h)

	# these are the individual tests
	file(GLOB ${APP}_SOURCES_TEST RELATIVE ${CMAKE_SOURCE_DIR} ../../MyProject/src/Tests/*.c;../../MyProject/src/Tests/*.cpp;)
	file(GLOB ${APP}_HEADERS_TEST RELATIVE ${CMAKE_SOURCE_DIR} ../../MyProject/src/Tests/*.h;*.hpp)
	source_group (Tests FILES ${APP}_SOURCES_TEST ${APP}_HEADERS_TEST)
	
	## add include directories
	add_includes ()

	add_executable(${APP} ${${APP}_SOURCES} ${${APP}_HEADERS} ${${APP}_SOURCES_TEST} ${${APP}_HEADERS_TEST})

	# link the libraries
	target_link_libraries(${APP} ${LIB} ${TestLIB} ${AudioLIB} ${SndLIB} ${OS_LIBS})
endif(WITH_TESTS)
