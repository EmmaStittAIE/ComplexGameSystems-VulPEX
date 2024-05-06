#!/usr/bin/env sh

# Runs when we fail to download a crucial library
# It is assumed that we are in "lib" when this is called
# 1 = GLM | 2 = GLFW
function failedDownLib {
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

        *)
            echo "Unknown error occured while downloading libraries"

    esac

    exit 1
}

function failedPremake {
    echo ""
    echo "---Premake Failed---"
    echo ""

    echo "Premake5 was not able to generate project files"

    exit
}

function failedBuild {
    echo ""
    echo "---Build Failed---"
    echo ""

    echo "Make was not able to build project"

    exit
}

echo "---Build Start---"

# Download libs
echo ""
echo "---Downloading Libraries---"
echo ""
cd ../lib

# GLM
if ! test -d glm; then
    echo "Cloning GLM..."

    if ! git clone https://github.com/g-truc/glm; then
        failedDownLib 1
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
        failedDownLib 2
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

echo ""
echo "---Building Projects---"
echo ""
# Premake
echo "Running premake..."
cd ..

if ! premake5 gmake2; then
    failedPremake
fi

echo "Premake complete"

# Make
echo "Running make"
cd generated

if ! make; then
    failedBuild
fi

echo "Make complete"

echo "Build complete"

if [ $1 == "-run" ] || [ $1 == "-r" ] || [ $1 == "-R" ]; then
    echo ""
    echo "---Running Main Project---"
    echo ""
    cd ..
    cd build/debug/TestApp/bin
    ./TestApp
fi
