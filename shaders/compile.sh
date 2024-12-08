#!/bin/sh

if [ $# -eq 0 ]; then
	echo "Path to SPIR-V compiler must be supplied!"
	exit
fi

${1} shader.vert -o vert.spv
${1} shader.frag -o frag.spv
