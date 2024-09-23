#!/usr/bin/env sh

rm SPIR-V/defaultVert.spv
rm SPIR-V/defaultFrag.spv

glslc GLSL/default.vert -o SPIR-V/defaultVert.spv
glslc GLSL/default.frag -o SPIR-V/defaultFrag.spv
