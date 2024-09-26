# ==================================================================================================
# CMake
# ==================================================================================================
cmake_minimum_required(VERSION 3.19)

# ==================================================================================================
# Project declaration
# ==================================================================================================
project(VizAPIs_Linux)
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
 set(CMAKE_HOST_SYSTEM_NAME "Windows") 
 endif() 
# ==================================================================================================
# CMake policies
# ==================================================================================================
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
    cmake_policy(SET CMP0074 NEW)
endif()

# ==================================================================================================
# Compiler check
# ==================================================================================================
set(MIN_CLANG_VERSION "6.0")

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    if (CMAKE_C_COMPILER_VERSION VERSION_LESS MIN_CLANG_VERSION)
        message(FATAL_ERROR "Detected C compiler Clang ${CMAKE_C_COMPILER_VERSION} < ${MIN_CLANG_VERSION}")
    endif()
else()
    message(FATAL_ERROR "Detected C compiler ${CMAKE_C_COMPILER_ID} is unsupported")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS MIN_CLANG_VERSION)
        message(FATAL_ERROR "Detected CXX compiler Clang ${CMAKE_CXX_COMPILER_VERSION} < ${MIN_CLANG_VERSION}")
    endif()
else()
    message(FATAL_ERROR "Detected CXX compiler ${CMAKE_CXX_COMPILER_ID} is unsupported")
endif()

# ==================================================================================================
# Support for ccache
# ==================================================================================================
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# ==================================================================================================
# Support Vim and Visual Studio Code by generating compile_commands.json
# ==================================================================================================
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==================================================================================================
# Configuration for CMAKE_CROSSCOMPILING.
# ==================================================================================================

set(IMPORT_EXECUTABLES ${CMAKE_CURRENT_SOURCE_DIR}/../../out/ImportExecutables-${CMAKE_BUILD_TYPE}.cmake)

# ==================================================================================================
# Common Functions
# ==================================================================================================

# Sets the following variables: RESGEN_HEADER, RESGEN_SOURCE, RESGEN_FLAGS, RESGEN_SOURCE_FLAGS,
# and RESGEN_OUTPUTS. Please pass in an ARCHIVE_NAME that is unique to your project, otherwise the
# incbin directive will happily consume a blob from the wrong project without warnings or errors.
# Also be sure to include the ASM language in the CMake "project" directive for your project.
function(get_resgen_vars ARCHIVE_DIR ARCHIVE_NAME)
    set(OUTPUTS
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.bin
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.S
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.apple.S
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.h
    )
    set(RESGEN_HEADER "${ARCHIVE_DIR}/${ARCHIVE_NAME}.h" PARENT_SCOPE)
    set(RESGEN_OUTPUTS "${OUTPUTS};${ARCHIVE_DIR}/${ARCHIVE_NAME}.c" PARENT_SCOPE)
    set(RESGEN_FLAGS -qcx ${ARCHIVE_DIR} -p ${ARCHIVE_NAME} PARENT_SCOPE)
    set(RESGEN_SOURCE "${ARCHIVE_DIR}/${ARCHIVE_NAME}.c" PARENT_SCOPE)
endfunction()

# ==================================================================================================
# Compile resources
# ==================================================================================================

set(GENERATION_ROOT ${CMAKE_CURRENT_BINARY_DIR}/../../API_SOURCE)
set(RESOURCE_DIR  "${GENERATION_ROOT}/generated/res")
set(MATERIAL_DIR  "${GENERATION_ROOT}/generated/mat")

set(MATERIAL_SRCS
        ../API_SOURCE/mat/compositor.mat
)

include(${IMPORT_EXECUTABLES})

file(MAKE_DIRECTORY ${MATERIAL_DIR})
file(MAKE_DIRECTORY ${RESOURCE_DIR})

set(RESOURCE_BINS)

set(MATC_BASE_FLAGS -a opengl -a vulkan -p desktop -g)

set (MATC_FLAGS ${MATC_BASE_FLAGS})
if (FILAMENT_SAMPLES_STEREO_TYPE STREQUAL "instanced")
    set (MATC_FLAGS ${MATC_FLAGS} -PstereoscopicType=instanced)
    add_definitions(-DFILAMENT_SAMPLES_STEREO_TYPE_INSTANCED)
elseif (FILAMENT_SAMPLES_STEREO_TYPE STREQUAL "multiview")
    set (MATC_FLAGS ${MATC_FLAGS} -PstereoscopicType=multiview)
    add_definitions(-DFILAMENT_SAMPLES_STEREO_TYPE_MULTIVIEW)
endif ()

foreach (mat_src ${MATERIAL_SRCS})
    get_filename_component(localname "${mat_src}" NAME_WE)
    get_filename_component(fullname "${mat_src}" ABSOLUTE)
    set(output_path "${MATERIAL_DIR}/${localname}.filamat")
    add_custom_command(
            OUTPUT ${output_path}
            COMMAND matc ${MATC_FLAGS} -o ${output_path} ${fullname}
            MAIN_DEPENDENCY ${mat_src}
            DEPENDS matc
            COMMENT "Compiling material ${mat_src} to ${output_path}"
    )
    list(APPEND RESOURCE_BINS ${output_path})
endforeach()

get_resgen_vars(${RESOURCE_DIR} mat_internal)

add_custom_command(
        OUTPUT ${RESGEN_OUTPUTS}
        COMMAND resgen ${RESGEN_FLAGS} ${RESOURCE_BINS}
        DEPENDS resgen ${RESOURCE_BINS}
        COMMENT "Aggregating resources"
)

if (DEFINED RESGEN_SOURCE_FLAGS)
    set_source_files_properties(${RESGEN_SOURCE} PROPERTIES COMPILE_FLAGS ${RESGEN_SOURCE_FLAGS})
endif()

# CMake fails to invoke ar on Windows unless there is at least one C/C++ file in the library.
set(DUMMY_SRC "${RESOURCE_DIR}/dummy.c")
add_custom_command(OUTPUT ${DUMMY_SRC} COMMAND echo "//" > ${DUMMY_SRC})

add_library(mat_internal ${DUMMY_SRC} ${RESGEN_SOURCE})

# ==================================================================================================
# General compiler flags
# ==================================================================================================
set(CXX_STANDARD "-std=c++20")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_STANDARD} -fstrict-aliasing -Wno-unknown-pragmas -Wno-unused-function -Wno-deprecated-declarations -fPIC")

option(USE_STATIC_LIBCXX "Link against the static runtime libraries." ON)
if (${USE_STATIC_LIBCXX})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    link_libraries("-static-libgcc -static-libstdc++")
    link_libraries(libc++.a)
    link_libraries(libc++abi.a)
endif()

# Add colors to ninja builds
if (UNIX AND CMAKE_GENERATOR STREQUAL "Ninja")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
endif()

# Use hidden by default and expose what we need.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

# ==================================================================================================
# Release compiler flags
# ==================================================================================================
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")

# ==================================================================================================
# Debug compiler flags
# ==================================================================================================
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fstack-protector")

# ==================================================================================================
# Linker flags
# ==================================================================================================
# Strip unused sections
set(GC_SECTIONS "-Wl,--gc-sections")

set(B_SYMBOLIC_FUNCTIONS "-Wl,-Bsymbolic-functions")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GC_SECTIONS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GC_SECTIONS} ${B_SYMBOLIC_FUNCTIONS} ${BINARY_ALIGNMENT}")

# ==================================================================================================
# Project flags
# ==================================================================================================
add_definitions(-DFILAMENT_SUPPORTS_OPENGL)

add_definitions(-DFILAMENT_DRIVER_SUPPORTS_VULKAN)

add_definitions(-DFILAMENT_DISABLE_MATOPT=1)

add_definitions(-DFILAMENT_SAMPLES_STEREO_TYPE_INSTANCED)

add_definitions(-DFILAMENT_IBL_LITE=1)

add_definitions(-DGLTFIO_DRACO_SUPPORTED=1)

option(FILAMENT_SUPPORTS_VULKAN "Enables Vulkan on Android" ON)
option(FILAMENT_ENABLE_MATDBG "Enables Material debugger" ON)

# ==================================================================================================
# Sources and headers
# ==================================================================================================

set(COMPONENT_HDRS
        ../API_SOURCE/components/VzActor.h
        ../API_SOURCE/components/VzAsset.h
        ../API_SOURCE/components/VzCamera.h
        ../API_SOURCE/components/VzFont.h
        ../API_SOURCE/components/VzGeometry.h
        ../API_SOURCE/components/VzLight.h
        ../API_SOURCE/components/VzMaterial.h
        ../API_SOURCE/components/VzMI.h
        ../API_SOURCE/components/VzRenderer.h
        ../API_SOURCE/components/VzScene.h
        ../API_SOURCE/components/VzSkeleton.h
        ../API_SOURCE/components/VzTexture.h
)

set(ROOT_HDRS
        ../API_SOURCE/VizComponentAPIs.h
        ../API_SOURCE/VizEngineAPIs.h
        ../API_SOURCE/VzComponents.h
)

set(PUBLIC_HDRS
        ${COMPONENT_HDRS}
        ${ROOT_HDRS}
)

set(SRCS
        ../API_SOURCE/backend/resource_internal.c
        ../API_SOURCE/backend/VzAssetExporter.cpp
        ../API_SOURCE/backend/VzAssetLoader.cpp
        ../API_SOURCE/backend/VzCube.cpp
        ../API_SOURCE/backend/VzIBL.cpp
        ../API_SOURCE/backend/VzMeshAssimp.cpp
        ../API_SOURCE/components/VzActor.cpp
        ../API_SOURCE/components/VzAsset.cpp
        ../API_SOURCE/components/VzCamera.cpp
        ../API_SOURCE/components/VzFont.cpp
        ../API_SOURCE/components/VzGeometry.cpp
        ../API_SOURCE/components/VzLight.cpp
        ../API_SOURCE/components/VzMaterial.cpp
        ../API_SOURCE/components/VzMI.cpp
        ../API_SOURCE/components/VzRenderer.cpp
        ../API_SOURCE/components/VzScene.cpp
        ../API_SOURCE/components/VzSkeleton.cpp
        ../API_SOURCE/components/VzTexture.cpp
        ../API_SOURCE/VizComponentAPIs.cpp
        ../API_SOURCE/VizCoreUtils.cpp
        ../API_SOURCE/VizEngineAPIs.cpp
        ../API_SOURCE/VzEngineApp.cpp
        ../API_SOURCE/VzRenderPath.cpp
)

set(PRIVATE_HDRS
        ../API_SOURCE/backend/resource_internal.h
        ../API_SOURCE/backend/VzAssetExporter.h
        ../API_SOURCE/backend/VzAssetLoader.h
        ../API_SOURCE/backend/VzConfig.h
        ../API_SOURCE/backend/VzCube.h
        ../API_SOURCE/backend/VzIBL.h
        ../API_SOURCE/backend/VzMeshAssimp.h
        ../API_SOURCE/FIncludes.h
        ../API_SOURCE/PreDefs.h
        ../API_SOURCE/VizCoreUtils.h
        ../API_SOURCE/VzEngineApp.h
        ../API_SOURCE/VzNameComponents.hpp
        ../API_SOURCE/VzRenderPath.h
)

# ==================================================================================================
# Includes & target definition
# ==================================================================================================
# specify where our headers are
include_directories(../API_SOURCE)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)

set(FILAMENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../out/${CMAKE_BUILD_TYPE_LOWER}/filament)

# we're building a library
add_library(${PROJECT_NAME} SHARED ${PRIVATE_HDRS} ${PUBLIC_HDRS} ${SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE mat_internal)

target_include_directories(${PROJECT_NAME} PUBLIC
    ../includes
    ../../libs/filabridge/include
    ../../filament/backend/include
    ../../filament/src
    ../../third_party/cgltf
    ../../out/${CMAKE_BUILD_TYPE_LOWER}/filament/include
    ../../libs/bluevk/include
    ../../libs/utils/include
    ../../libs/filamentapp/include
    ../../third_party/stb
    ../../third_party/libassimp/include
    ../../out/cmake-${CMAKE_BUILD_TYPE_LOWER}/samples
    ../../out/cmake-${CMAKE_BUILD_TYPE_LOWER}/libs/gltfio
)

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)

# ==================================================================================================
# Dependencies
# ==================================================================================================


target_link_libraries(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/libfreetype.a
    ${FILAMENT_DIR}/lib/x86_64/libfilament.a
    ${FILAMENT_DIR}/lib/x86_64/libbackend.a
    ${FILAMENT_DIR}/lib/x86_64/libfilaflat.a
    ${FILAMENT_DIR}/lib/x86_64/libfilabridge.a
    ${FILAMENT_DIR}/lib/x86_64/libbluegl.a
    ${FILAMENT_DIR}/lib/x86_64/libbluevk.a
    ${FILAMENT_DIR}/lib/x86_64/libcamutils.a
    ${FILAMENT_DIR}/lib/x86_64/libcivetweb.a

    ${FILAMENT_DIR}/../../cmake-${CMAKE_BUILD_TYPE_LOWER}/samples/libgltf-demo-resources.a
    ${FILAMENT_DIR}/../../cmake-${CMAKE_BUILD_TYPE_LOWER}/samples/libsample-resources.a
    ${FILAMENT_DIR}/../../cmake-${CMAKE_BUILD_TYPE_LOWER}/samples/libsuzanne-resources.a

    ${FILAMENT_DIR}/lib/x86_64/libfilameshio.a
    ${FILAMENT_DIR}/lib/x86_64/libgltfio.a
    ${FILAMENT_DIR}/lib/x86_64/libgltfio_core.a

    ${FILAMENT_DIR}/lib/x86_64/libmatdbg.a
    ${FILAMENT_DIR}/lib/x86_64/libfilamat.a

    ${FILAMENT_DIR}/lib/x86_64/libviewer.a
    ${FILAMENT_DIR}/lib/x86_64/libuberarchive.a
    ${FILAMENT_DIR}/lib/x86_64/libuberzlib.a
    ${FILAMENT_DIR}/lib/x86_64/libgeometry.a
    ${FILAMENT_DIR}/lib/x86_64/libdracodec.a
    ${FILAMENT_DIR}/lib/x86_64/libibl.a
    ${FILAMENT_DIR}/lib/x86_64/libibl-lite.a
    ${FILAMENT_DIR}/lib/x86_64/libfilament-iblprefilter.a
    ${FILAMENT_DIR}/lib/x86_64/libstb.a

    ${FILAMENT_DIR}/lib/x86_64/libktxreader.a
    ${FILAMENT_DIR}/lib/x86_64/libmeshoptimizer.a
    ${FILAMENT_DIR}/../../cmake-${CMAKE_BUILD_TYPE_LOWER}/libs/imageio/libimageio.a
    ${FILAMENT_DIR}/lib/x86_64/libimage.a
    ${FILAMENT_DIR}/lib/x86_64/libmikktspace.a
    ${FILAMENT_DIR}/lib/x86_64/libshaders.a
    ${FILAMENT_DIR}/lib/x86_64/libsmol-v.a
    ${FILAMENT_DIR}/lib/x86_64/libvkshaders.a

    ${FILAMENT_DIR}/lib/x86_64/libutils.a
    ${FILAMENT_DIR}/../../cmake-${CMAKE_BUILD_TYPE_LOWER}/third_party/libpng/tnt/libpng.a
    ${FILAMENT_DIR}/../../cmake-${CMAKE_BUILD_TYPE_LOWER}/third_party/tinyexr/tnt/libtinyexr.a
    ${FILAMENT_DIR}/lib/x86_64/libbasis_transcoder.a
    ${FILAMENT_DIR}/lib/x86_64/libzstd.a
    ${FILAMENT_DIR}/../../cmake-${CMAKE_BUILD_TYPE_LOWER}/third_party/libz/tnt/libz.a
    ${FILAMENT_DIR}/../../cmake-${CMAKE_BUILD_TYPE_LOWER}/third_party/libassimp/tnt/libassimp.a
)

# ==============================================================================================
# Installation
# ==============================================================================================
install(FILES ${COMPONENT_HDRS} DESTINATION include/components)
install(FILES ${ROOT_HDRS} DESTINATION include)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
