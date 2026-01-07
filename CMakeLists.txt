cmake_minimum_required(VERSION 3.28)

project(AcousticFluids 
    VERSION 0.1 
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(AcousticFluids) 

target_sources(AcousticFluids PRIVATE
    src/main.cpp
    src/Core/Clock.hpp
    src/Core/Clock.cpp
    src/Core/Config.hpp
    src/Core/Engine.hpp
    src/Core/Engine.cpp
    src/Core/Logger.hpp
    src/Core/Logger.cpp
    src/Core/Window.hpp
    src/Core/Window.cpp
    src/Graphics/GPUContext.hpp
    src/Graphics/GPUContext.cpp
    src/Graphics/Renderer.hpp
    src/Graphics/Renderer.cpp
)

target_include_directories(AcousticFluids PRIVATE src)
target_link_libraries(AcousticFluids PRIVATE SDL3::SDL3 spdlog::spdlog)

if(MSVC)
    target_compile_options(AcousticFluids PRIVATE /W4 /permissive- /Zc:__cplusplus /EHsc /utf-8)
else()
    target_compile_options(AcousticFluids PRIVATE -Wall -Wextra -Wpedantic)
endif()
