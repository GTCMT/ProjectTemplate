
if (NOT GTCMT_WIN32 AND NOT GTCMT_WIN64)
    find_package (Threads)
    set(PTHREAD_LIBRARY ${CMAKE_THREAD_LIBS_INIT})
    #set(PTHREAD_INCLUDE_DIR )
else (NOT GTCMT_WIN32 AND NOT GTCMT_WIN64)
    IF( NOT PTHREADS )
        FILE(GLOB PTHREAD_INCLUDE_PATHS "${CMAKE_SOURCE_DIR}/3rdPartyLibs/pthreads-Win32/include")

        if (GTCMT_WIN64)
            set(OS_BIT "x64")
        endif (GTCMT_WIN64)
        if (GTCMT_WIN32)
            set(OS_BIT "x86")
        endif (GTCMT_WIN32)
        FILE(GLOB PTHREAD_LIBRARY_PATHS "${CMAKE_SOURCE_DIR}/3rdPartyLibs/pthreads-Win32/lib/${OS_BIT}")

        FIND_PATH(
          PTHREAD_INCLUDE_DIR
          pthread.h
          ${PTHREAD_INCLUDE_PATHS}
        )

        set (PTHREAD_NAMES "pthreadVC2")
        FIND_LIBRARY( PTHREAD_LIBRARY  
            NAMES ${PTHREAD_NAMES} 	 
            PATHS ${PTHREAD_LIBRARY_PATHS}  )
        
        include (FindPackageHandleStandardArgs)
        find_package_handle_standard_args(PTHREADS DEFAULT_MSG PTHREAD_INCLUDE_DIR PTHREAD_LIBRARY)
    ENDIF( NOT PTHREADS )
endif (NOT GTCMT_WIN32 AND NOT GTCMT_WIN64)

