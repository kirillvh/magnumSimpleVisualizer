cmake_minimum_required(VERSION 3.0)

project(magnumSimpleVisualizer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/dep/magnum/modules/")

add_subdirectory(dep/corrade)
add_subdirectory(dep/magnum)

OPTION(DEBUG_BUILD "Debug/Release build" ON)

Set(Proj_VERSION_MAJOR 0)
Set(Proj_VERSION_MINOR 1)
Set(Proj_VERSION_TIMESTAMP 20190501)

add_definitions(-DProj_VERSION_MAJOR=${Proj_VERSION_MAJOR})
add_definitions(-DProj_VERSION_MINOR=${Proj_VERSION_MINOR})
add_definitions(-DProj_VERSION_TIMESTAMP=${Proj_VERSION_TIMESTAMP})

find_package(SDL2 REQUIRED)
find_package(Magnum REQUIRED
    GL
    MeshTools
    Primitives
    Shaders
    Sdl2Application)
find_package(Magnum REQUIRED Sdl2Application)
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

## currently using target_include_directories
include_directories(
${PROJECT_SOURCE_DIR}/src
${PROJECT_SOURCE_DIR}/h
${Dependencies}
${SDL2_INCLUDE_DIR}
)


file(GLOB SRCS "src/*.cpp"   )


corrade_add_resource(Picking_RESOURCES src/resources.conf)
add_executable (App ${SRCS} ${Picking_RESOURCES})


TARGET_LINK_LIBRARIES(App PRIVATE
  Magnum::Application
  Magnum::GL
  Magnum::Magnum
  Magnum::MeshTools
  Magnum::Primitives
  Magnum::SceneGraph
  Magnum::Shaders
)

target_compile_options(App PRIVATE "-std=c++17" "-Wall" "-o0" "-g")
