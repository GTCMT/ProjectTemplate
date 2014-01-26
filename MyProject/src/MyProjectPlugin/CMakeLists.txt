set(CurrLib MyProjectPlugin)

# get all source files
set(LocalDir ${CMAKE_SOURCE_DIR}/MyProject/src/${CurrLib})
file(GLOB ${CurrLib}_SOURCES RELATIVE ${LocalDir} *.cpp;*.txt)
file(GLOB ${CurrLib}_HEADERS RELATIVE ${LocalDir} *.h;../../incl/*.h)

## add include directories
add_includes ()

option (PLUGIN_VST "build a VST plugin" ON)
if (APPLE)
	option (PLUGIN_AU "build an AU plugin" ON)
endif (APPLE)

if (PLUGIN_VST)
	find_package(VST REQUIRED)

	if (VST_FOUND)
		include_directories(${VST_INCLUDE_DIR})
		file(GLOB VST_SOURCES RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/VST/*.cpp)
		file(GLOB VST_HEADERS RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/VST/*.h;)
	endif (VST_FOUND)
endif (PLUGIN_VST)
if (PLUGIN_AU)
endif (PLUGIN_AU)

file(GLOB UTILITY_SOURCES RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/utility/*.cpp)
file(GLOB UTILITY_HEADERS RELATIVE ${LocalDir} ${JUCE_PLUGIN_WRAPPER_DIR}/utility/*.h;)	

set(JUCE_PLUGIN_SOURCES ${VST_SOURCES} ${AUDIOUNIT_SOURCES} ${UTILITY_SOURCES})
set(JUCE_PLUGIN_HEADERS ${VST_HEADERS} ${AUDIOUNIT_HEADERS} ${UTILITY_HEADERS})

# build the library
add_library(${CurrLib} SHARED ${${CurrLib}_SOURCES} ${${CurrLib}_HEADERS} ${JUCE_PLUGIN_SOURCES} ${JUCE_PLUGIN_HEADERS})

set_target_properties(${CurrLib} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${CurrLib} PROPERTIES OUTPUT_NAME "${CurrLib}")

target_link_libraries(${CurrLib} MyProject JuceLibraryCode)

option (WITH_CPPCHECKTARGET "Add target for cppcheck testing" ON)
if (WITH_CPPCHECKTARGET)
	# cppcheck target in the IDE
	file(GLOB to_remove RELATIVE ${CMAKE_SOURCE_DIR}/${CurrLib}/src/${CurrLib} *.txt)
	list(REMOVE_ITEM ${CurrLib}_SOURCES ${to_remove})
	include (GenerateCppcheck)
	GENERATE_CPPCHECK(SOURCES ${${CurrLib}_SOURCES}
						TARGET_NAME "CppCheck_${CurrLib}"
					  INCLUDES ${PROJECT_INCLUDES})
endif(WITH_CPPCHECKTARGET)