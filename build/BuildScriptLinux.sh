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
    echo "Usage: $0 [ -c CONFIG] [ -p PLATFORM ] [ -s SYSTEM ] [ -r ] [ -d ]"
}

# Define default options
run=false
debug=false
config="debug"
platform="linux"
system="make"

# getopts loop
while getopts "hrdc:p:s:" options; do

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

        s)
            system=${OPTARG}
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
doCloneLib=false;
if test -f glm/.hash.txt; then
    localHash=$(head glm/.hash.txt)
    remoteHash=$(git ls-remote https://github.com/g-truc/glm HEAD | awk '{print $1}')

    if [[ "$localHash" != "$remoteHash" ]]; then
        doCloneLib=true;
    fi
else
    doCloneLib=true;
fi

if [[ $doCloneLib == true ]]; then
    rm -rf glm

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

    git ls-remote https://github.com/g-truc/glm HEAD | awk '{print $1}' > glm/.hash.txt

    echo "GLM is ready"
else
    echo "GLM already exists, skipping..."
fi

# GLFW
doCloneLib=false;
if test -f GLFW/.hash.txt; then
    localHash=$(head GLFW/.hash.txt)
    remoteHash=$(git ls-remote https://github.com/glfw/glfw HEAD | awk '{print $1}')

    if [[ "$localHash" != "$remoteHash" ]]; then
        doCloneLib=true;
    fi
else
    doCloneLib=true;
fi

if [[ $doCloneLib == true ]]; then
    rm -rf GLFW

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

    git ls-remote https://github.com/glfw/glfw HEAD | awk '{print $1}' > GLFW/.hash.txt

    echo "GLFW is ready"
else
    echo "GLFW already exists, skipping..."
fi

# EmmaUtils
doCloneLib=false;
if test -f EmmaUtils/.hash.txt; then
    localHash=$(head EmmaUtils/.hash.txt)
    remoteHash=$(git ls-remote https://github.com/EmmaStittAIE/EmmaUtils HEAD | awk '{print $1}')

    if [[ "$localHash" != "$remoteHash" ]]; then
        doCloneLib=true;
    fi
else
    doCloneLib=true;
fi

if [[ $doCloneLib == true ]]; then
    rm -rf EmmaUtils

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

    git ls-remote https://github.com/EmmaStittAIE/EmmaUtils HEAD | awk '{print $1}' > EmmaUtils/.hash.txt

    echo "EmmaUtils is ready"
else
    echo "EmmaUtils already exists, skipping..."
fi

echo ""
echo "---Building Projects---"
echo ""

# Shaders
echo "Compiling shaders..."
cd ../working/Assets/Shaders
./CompileShaders.sh

echo "Shaders compiled"

# Premake
echo "Running premake..."
cd ../../..

if [[ ${system} == "Make" ]]; then
    if ! premake5 gmake2; then
        FailedPremake
    fi
elif [[ ${system} == "VS2022" ]]; then
    if ! premake5 vs2022; then
        FailedPremake
    fi
else
    FailedPremake
fi

echo "Premake complete"

if [[ ${system} == "Make" ]]; then
    # Make
    echo "Running make"
    cd generated

    if ! make config=$buildConfig; then
        FailedBuild
    fi

    echo "Make complete"

    echo "Build complete"
fi

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
        lldb TestApp 2>&1
    else
        ./TestApp 2>&1
    fi
fi
