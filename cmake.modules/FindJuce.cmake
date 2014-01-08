
IF( NOT JUCE_FOUND )

	FIND_PATH(JUCE_ROOT_DIR juce.h PATHS ${CMAKE_SOURCE_DIR}/3rdPartyLibs/JUCE )
#	FIND_PATH(JUCE_APP_CONFIG_DIR AppConfig.h PATHS ${CMAKE_SOURCE_DIR}/JuceLibraryCode)

	set(JUCE_MODULES juce_core juce_data_structures juce_events juce_graphics juce_gui_basics juce_gui_extra juce_audio_basics juce_audio_devices juce_audio_formats juce_audio_plugin_client juce_audio_processors juce_audio_utils)
	
	foreach(dir ${JUCE_MODULES})
		if (APPLE)
			file(GLOB JUCE_SOURCES_${dir} RELATIVE ${CMAKE_SOURCE_DIR}/MyProject/src/MyProject ${JUCE_ROOT_DIR}/modules/${dir}/*.mm )
		else (APPLE)
			file(GLOB JUCE_SOURCES_${dir} RELATIVE ${CMAKE_SOURCE_DIR}/MyProject/src/MyProject ${JUCE_ROOT_DIR}/modules/${dir}/*.cpp )
		endif (APPLE)
		file(GLOB JUCE_HEADERS_${dir} RELATIVE ${CMAKE_SOURCE_DIR}/MyProject/src/MyProject ${JUCE_ROOT_DIR}/modules/${dir}/*.h )
		
	endforeach(dir ${JUCE_MODULES})


	include (FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Juce DEFAULT_MSG JUCE_ROOT_DIR)
	#MESSAGE(STATUS "Juce: ${JUCE_SOURCES}")

ENDIF( NOT JUCE_FOUND )

