cmake_minimum_required(VERSION 3.16)
project(Platformer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Platformer ${SOURCES})


find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
target_link_libraries(Platformer sfml-graphics sfml-window sfml-system sfml-audio)


add_custom_command(TARGET Platformer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Platformer>/assets
)