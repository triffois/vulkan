#!/bin/sh

if [ $# -eq 0 ]; then
	echo "Path to SPIR-V compiler must be supplied!"
	exit
fi

# Compile original shaders
${1} shader.vert -o vert.spv
${1} shader.frag -o frag.spv

# Compile raymarching shaders
${1} raymarching.vert -o raymarching_vert.spv
${1} raymarching.frag -o raymarching_frag.spv
