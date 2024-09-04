#!/bin/bash
set -e

MOBILE_HOST_TOOLS="matc resgen cmgen filamesh uberz"

function print_help {
    local self_name=$(basename "$0")
    echo "Usage:"
    echo "    $self_name [options] <build_type1> [<build_type2> ...]"
    echo ""
    echo "Options:"
    echo "    -h"
    echo "        Print this help message."
    echo "    -c"
    echo "        Clean build directories."
    echo "    -f"
    echo "        Always invoke CMake before incremental builds."
    echo "    -q abi1,abi2,..."
    echo "        Where platformN is [armeabi-v7a|arm64-v8a|x86|x86_64|all]."
    echo "        ABIs to build when the platform is Android. Defaults to all."
    echo ""
    echo "Build types:"
    echo "    release"
    echo "        Release build only"
    echo "    debug"
    echo "        Debug build only"
    echo ""
    echo "Examples:"
    echo "    Desktop release build:"
    echo "        \$ ./$self_name release"
    echo ""
    echo "    Desktop debug and release builds:"
    echo "        \$ ./$self_name debug release"
    echo ""
 }

# Requirements
CMAKE_MAJOR=3
CMAKE_MINOR=19

# Internal variables
ISSUE_CLEAN=false

ISSUE_DEBUG_BUILD=false
ISSUE_RELEASE_BUILD=false

# Default: all
ABI_ARMEABI_V7A=true
ABI_ARM64_V8A=true
ABI_X86=true
ABI_X86_64=true

ISSUE_CMAKE_ALWAYS=false

INSTALL_COMMAND=install

BUILD_GENERATOR=Ninja
BUILD_COMMAND=ninja

# Functions

function build_clean {
    echo "Cleaning build directories..."
    rm -Rf cmake-android-*
    rm -Rf dist
}

function build_android_target {
    local lc_target=$(echo "$1" | tr '[:upper:]' '[:lower:]')
    local arch=$2

    echo "Building Android ${lc_target} (${arch})..."
    mkdir -p "cmake-android-${lc_target}-${arch}"

    pushd "cmake-android-${lc_target}-${arch}" > /dev/null

    if [[ ! -d "CMakeFiles" ]] || [[ "${ISSUE_CMAKE_ALWAYS}" == "true" ]]; then
        cmake \
            -G "${BUILD_GENERATOR}" \
            -DCMAKE_BUILD_TYPE="${lc_target}" \
            -DFILAMENT_NDK_VERSION="${FILAMENT_NDK_VERSION}" \
            -DCMAKE_INSTALL_PREFIX="../dist/${arch}" \
            -DCMAKE_TOOLCHAIN_FILE="../../build/toolchain-${arch}-linux-android.cmake" \
            ..
    fi

    ${BUILD_COMMAND} install

    popd > /dev/null
}

function build_android_arch {
    local arch=$1

    if [[ "${ISSUE_DEBUG_BUILD}" == "true" ]]; then
        build_android_target "Debug" "${arch}"
    fi

    if [[ "${ISSUE_RELEASE_BUILD}" == "true" ]]; then
        build_android_target "Release" "${arch}"
    fi
}

function ensure_android_build {
    if [[ "${ANDROID_HOME}" == "" ]]; then
        echo "Error: ANDROID_HOME is not set, exiting"
        exit 1
    fi

    # shellcheck disable=SC2012
    if [[ -z $(ls "${ANDROID_HOME}/ndk/" | sort -V | grep "^${FILAMENT_NDK_VERSION}") ]]; then
        echo "Error: Android NDK side-by-side version ${FILAMENT_NDK_VERSION} or compatible must be installed, exiting"
        exit 1
    fi

    local cmake_version=$(cmake --version)
    if [[ "${cmake_version}" =~ ([0-9]+)\.([0-9]+)\.[0-9]+ ]]; then
        if [[ "${BASH_REMATCH[1]}" -lt "${CMAKE_MAJOR}" ]] || \
           [[ "${BASH_REMATCH[2]}" -lt "${CMAKE_MINOR}" ]]; then
            echo "Error: cmake version ${CMAKE_MAJOR}.${CMAKE_MINOR}+ is required," \
                 "${BASH_REMATCH[1]}.${BASH_REMATCH[2]} installed, exiting"
            exit 1
        fi
    fi
}

function build_android {
    ensure_android_build

    if [[ "${ABI_ARM64_V8A}" == "true" ]]; then
        build_android_arch "aarch64" "aarch64-linux-android"
    fi
    if [[ "${ABI_ARMEABI_V7A}" == "true" ]]; then
        build_android_arch "arm7" "arm-linux-androideabi"
    fi
    if [[ "${ABI_X86_64}" == "true" ]]; then
        build_android_arch "x86_64" "x86_64-linux-android"
    fi
    if [[ "${ABI_X86}" == "true" ]]; then
        build_android_arch "x86" "i686-linux-android"
    fi
}

function validate_build_command {
    set +e
    # Make sure CMake is installed
    local cmake_binary=$(command -v cmake)
    if [[ ! "${cmake_binary}" ]]; then
        echo "Error: could not find cmake, exiting"
        exit 1
    fi

    # Make sure Ninja is installed
    if [[ "${BUILD_COMMAND}" == "ninja" ]]; then
        local ninja_binary=$(command -v ninja)
        if [[ ! "${ninja_binary}" ]]; then
            echo "Warning: could not find ninja, using make instead"
            BUILD_GENERATOR="Unix Makefiles"
            BUILD_COMMAND="make"
        fi
    fi

    set -e
}

function check_debug_release_build {
    if [[ "${ISSUE_DEBUG_BUILD}" == "true" || \
          "${ISSUE_RELEASE_BUILD}" == "true" || \
          "${ISSUE_CLEAN}" == "true" ]]; then
        "$@";
    else
        echo "You must declare a debug or release target for $@ builds."
        echo ""
        exit 1
    fi
}

# Beginning of the script

pushd "$(dirname "$0")" > /dev/null

while getopts ":hcf" opt; do
    case ${opt} in
        h)
            print_help
            exit 0
            ;;
        c)
            ISSUE_CLEAN=true
            ;;
        f)
            ISSUE_CMAKE_ALWAYS=true
            ;;
        \?)
            echo "Invalid option: -${OPTARG}" >&2
            echo ""
            print_help
            exit 1
            ;;
        :)
            echo "Option -${OPTARG} requires an argument." >&2
            echo ""
            print_help
            exit 1
            ;;
    esac
done

if [[ "$#" == "0" ]]; then
    print_help
    exit 1
fi

shift $((OPTIND - 1))

for arg; do
    if [[ $(echo "${arg}" | tr '[:upper:]' '[:lower:]') == "release" ]]; then
        ISSUE_RELEASE_BUILD=true
    elif [[ $(echo "${arg}" | tr '[:upper:]' '[:lower:]') == "debug" ]]; then
        ISSUE_DEBUG_BUILD=true
    else
        BUILD_CUSTOM_TARGETS="${BUILD_CUSTOM_TARGETS} ${arg}"
    fi
done

validate_build_command

if [[ "${ISSUE_CLEAN}" == "true" ]]; then
    build_clean
fi

check_debug_release_build build_android
