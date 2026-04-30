cmake_minimum_required(VERSION 3.15)

project(scorehud VERSION 8.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Change this to the Endstone version you are targeting
set(ENDSTONE_API_VERSION "0.11" CACHE STRING "Endstone API version")

include(FetchContent)
FetchContent_Declare(
        endstone
        GIT_REPOSITORY https://github.com/EndstoneMC/endstone.git
        GIT_TAG v${ENDSTONE_API_VERSION}
)
FetchContent_MakeAvailable(endstone)

if(TARGET fmt)
    target_compile_options(fmt PRIVATE -include cstdlib)
endif()

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
configure_file(include/version.h.in include/version.h)

endstone_add_plugin(${PROJECT_NAME}
        src/plugin.cpp
        src/listener.cpp
        src/scorehud/score_tag.cpp
        src/scorehud/scoreboard_view.cpp
        src/scorehud/session_manager.cpp
        src/scorehud/settings.cpp
        src/scorehud/tag_resolver.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include)
