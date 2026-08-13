cmake_minimum_required(VERSION 3.16)
project(GameSDL VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB SRC_FILES ${SRC_DIR}/*.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/SDL2-2.30.8/include
    ${CMAKE_SOURCE_DIR}/SDL2_image-2.8.2/include
    ${CMAKE_SOURCE_DIR}/SDL2_ttf-2.20.2/include
    ${CMAKE_SOURCE_DIR}/SDL2_mixer-2.8.0/include
)

add_executable(GameSDL ${SRC_FILES})

if(MSVC)
    set(SDL2_LIB_DIR ${CMAKE_SOURCE_DIR}/SDL2-2.30.8/lib/x64)
    set(SDL2_IMAGE_LIB_DIR ${CMAKE_SOURCE_DIR}/SDL2_image-2.8.2/lib/x64)
    set(SDL2_TTF_LIB_DIR ${CMAKE_SOURCE_DIR}/SDL2_ttf-2.20.2/lib/x64)
    set(SDL2_MIXER_LIB_DIR ${CMAKE_SOURCE_DIR}/SDL2_mixer-2.8.0/lib/x64)

    target_link_directories(GameSDL PRIVATE ${SDL2_LIB_DIR} ${SDL2_IMAGE_LIB_DIR} ${SDL2_TTF_LIB_DIR} ${SDL2_MIXER_LIB_DIR})
    target_link_libraries(GameSDL PRIVATE SDL2main SDL2 SDL2_image SDL2_ttf SDL2_mixer)

    add_custom_command(TARGET GameSDL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL2_LIB_DIR}/SDL2.dll
            ${SDL2_IMAGE_LIB_DIR}/SDL2_image.dll
            ${SDL2_TTF_LIB_DIR}/SDL2_ttf.dll
            ${SDL2_MIXER_LIB_DIR}/SDL2_mixer.dll
            $<TARGET_FILE_DIR:GameSDL>
    )
else()
    find_package(SDL2 REQUIRED)
    target_include_directories(GameSDL PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(GameSDL PRIVATE ${SDL2_LIBRARIES})
endif()
