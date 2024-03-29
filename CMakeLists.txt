cmake_minimum_required(VERSION 3.26)
project(raycaster)

set(SDL2_INCLUDE_DIR C:/dev/SDL2-2.30.0/include)
set(SDL2_LIB_DIR C:/dev/SDL2-2.30.0/lib/x64)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})


add_executable(${PROJECT_NAME}
        src/main.c
)

target_link_libraries(${PROJECT_NAME} SDL2main.lib SDL2.lib)

#find_package(SDL3 REQUIRED)
#target_include_directories(${PROJECT_NAME}
#        PUBLIC ${SDL3_INCLUDE_DIRS}
#)
#
#if(WIN32 AND NOT MSVC)
#    target_link_libraries(${PROJECT_NAME}
#            PUBLIC ${SDL3_LIBRARIES} mingw32
#    )
#else()
#    target_link_libraries(${PROJECT_NAME}
#            PUBLIC ${SDL3_LIBRARIES}
#    )
#endif ()
