## set defines for operating system and platform

if (APPLE)
	set(GTCMT_MACOSX 1)
endif (APPLE)

if (UNIX AND NOT APPLE)
	## get bit depth
	if ( CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(GTCMT_LINUX32 1)
	else ( CMAKE_SIZEOF_VOID_P EQUAL 4 )
		set(GTCMT_LINUX64 1)
	endif ( CMAKE_SIZEOF_VOID_P EQUAL 4 )
endif (UNIX AND NOT APPLE)

if (MSVC)
	if ( CMAKE_CL_64 EQUAL 0 )
		set(GTMCT_WIN32 1)
	else ( CMAKE_CL_64 EQUAL 0 )
		set(GTMCT_WIN64 1)
	endif ( CMAKE_CL_64 EQUAL 0 )
endif (MSVC)