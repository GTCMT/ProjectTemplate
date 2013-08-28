set (LIB MyProject)
set (APP ${LIB}CL)

# get all source files
file(GLOB ${APP}_SOURCES RELATIVE ${CMAKE_SOURCE_DIR}/${APP} *.c;*.cpp;*.txt)
file(GLOB ${APP}_HEADERS RELATIVE ${CMAKE_SOURCE_DIR}/${APP} *.h;*.hpp;../incl/*.h;../incl/*.hpp)

## add include directories
foreach(PATH ${PROJECT_INCLUDES})
    include_directories(${PATH})
endforeach(PATH) 

add_executable(${APP} ${${APP}_SOURCES} ${${APP}_HEADERS})

# link the libraries
target_link_libraries(${APP} ${LIB} ${OS_LIBS})
