#!/usr/bin/env sh

# Runs when we fail to download a crucial library
# It is assumed that we are in "lib" when this is called
# 1 = GLM | 2 = GLFW | 3 = EmmaUtils
function FailedDownLib {
    echo ""
    echo "---Download Failed---"
    echo ""

    case $1 in

        1)
            echo "Failed to clone GLM"
            rm -rf glm
            ;;

        2)
            echo "Failed to clone GLFW"
            rm -rf glfw
            ;;

        2)
            echo "Failed to clone EmmaUtils"
            rm -rf EmmaUtils
            ;;

        *)
            echo "Unknown error occured while downloading libraries"

    esac

    exit 2
}

function FailedPremake {
    echo ""
    echo "---Premake Failed---"
    echo ""

    echo "Premake5 was not able to generate project files"

    exit 3
}

function FailedBuild {
    echo ""
    echo "---Build Failed---"
    echo ""

    echo "Make was not able to build project"

    exit 4
}

function CannotRun {
    echo ""
    echo "---Failed to Run---"
    echo ""

    echo "Cannot run executable of platform type \"$platform\""

    exit 5
}

function PrintHelp {
    echo "Usage: $0 [ -c CONFIG] [ -p PLATFORM ] [ -r ] [ -d ]"
}

# Define default options
run=false
debug=false
config="debug"
platform="linux"

# getopts loop
while getopts "hrdc:p:" options; do

    case "${options}" in

        h)
            PrintHelp
            exit 0
            ;;

        r)
            run=true
            ;;

        d)
            debug=true
            ;;

        c)
            config=${OPTARG}
            ;;

        p)
            platform=${OPTARG}
            ;;

        *)
            PrintHelp
            exit 1
            ;;
    esac
done

buildConfig="${config,,}_${platform,,}"

echo "---Build Start---"

# Download libs
echo ""
echo "---Downloading Libraries---"
echo ""
cd ..

mkdir lib
cd lib

# GLM
if ! test -d glm; then
    echo "Cloning GLM..."

    if ! git clone https://github.com/g-truc/glm; then
        FailedDownLib 1
    fi

    echo "Prepping GLM..."

    # Move the parts of GLM we want into "lib/glm" and delete the rest
    mv glm glmGit
    cd glmGit

    mv glm ..

    cd ..
    rm -rf glmGit

    echo "GLM is ready"
else
    echo "GLM already exists, skipping..."
fi

# GLFW
if ! test -d GLFW; then
    echo "Cloning GLFW..."

    if ! git clone https://github.com/glfw/glfw; then
        FailedDownLib 2
    fi

    echo "Building GLFW..."
    mv glfw glfwGit

    # Folder structure we'll need later
    mkdir GLFW
    mkdir GLFW/include

    cd glfwGit

    # Build GLFW
    cmake -S . -B build
    cd build
    make

    echo "Prepping GLFW..."
    # Move build to "GLFW"
    cd src
    mv libglfw3.a ../../../GLFW/libglfw3.a

    # Move headers to "GLFW/include"
    cd ../../include/GLFW
    mv glfw3.h ../../../GLFW/include/glfw3.h
    mv glfw3native.h ../../../GLFW/include/glfw3native.h

    cd ../../..
    rm -rf glfwGit

    echo "GLFW is ready"
else
    echo "GLFW already exists, skipping..."
fi

if ! test -d EmmaUtils; then
    echo "Cloning EmmaUtils..."

    if ! git clone https://github.com/EmmaStittAIE/EmmaUtils; then
        FailedDownLib 3
    fi

    echo "Building EmmaUtils..."
    mv EmmaUtils EmmaUtilsGit

    cd EmmaUtilsGit

    premake5 gmake2

    cd generated

    make config=debug_linux
    make config=release_linux

    echo "Prepping EmmaUtils..."
    cd ../build/Linux
    mv Debug ../../../EmmaUtils

    cd Release
    mv libEmmaUtils.a ../../../../EmmaUtils

    cd ../../../../EmmaUtils
    rm -rf obj

    cd include
    mv EmmaUtils ..

    cd ..
    rm -rf include
    mv EmmaUtils include

    cd ..
    rm -rf EmmaUtilsGit
fi

echo ""
echo "---Building Projects---"
echo ""

# Premake
echo "Running premake..."
cd ..

if ! premake5 gmake2; then
    FailedPremake
fi

echo "Premake complete"

# Make
echo "Running make"
cd generated

if ! make config=$buildConfig; then
    FailedBuild
fi

echo "Make complete"

echo "Build complete"

if [[ $run == true ]]; then

    if [[ ${platform} != "Linux" ]]; then
        CannotRun
    fi

    echo ""
    echo "---Running Main Project---"
    echo ""
    cd ..
    cd build/$platform/$config/TestApp/bin

    if [[ $debug == true ]]; then
        gdb TestApp
    else
        ./TestApp
    fi
fi
