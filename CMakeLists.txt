cmake_minimum_required(VERSION 3.16)
project(Platformer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Fetch SFML automatically ──────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.2          # locked to exact version — no surprises
        GIT_SHALLOW    TRUE           # faster download, no full git history
)

set(SFML_BUILD_AUDIO    ON)
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_WINDOW   ON)
set(SFML_BUILD_NETWORK  OFF)      # you don't need network
set(BUILD_SHARED_LIBS   OFF)      # static linking — no .dll/.so files to ship

FetchContent_MakeAvailable(SFML)
# ─────────────────────────────────────────────────────────────────────

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Platformer ${SOURCES})

target_include_directories(Platformer PRIVATE src/)

target_link_libraries(Platformer
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)

add_custom_command(TARGET Platformer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Platformer>/assets
)