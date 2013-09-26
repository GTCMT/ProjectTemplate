set (LIB MyProject)
set (APP ${LIB}CL)

# get all source files
file(GLOB ${APP}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/${APP} *.c;*.cpp;*.txt)
file(GLOB ${APP}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/${APP} *.h;../incl/*.h;../../../incl/SharedSources/incl/*.h)

## add include directories
add_includes ()

add_executable(${APP} ${${APP}_SOURCES} ${${APP}_HEADERS})

# link the libraries
target_link_libraries(${APP} ${LIB} AudioFileIO ${OS_LIBS})
