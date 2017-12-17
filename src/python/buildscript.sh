#!/bin/bash
# The arguments should be passed in this format:
# buildscript.sh <libseawolf's python directory> <python executable> [command(s) for setup.py]
fst="$1"
snd="$2"
# Get the third argument onwards 
rst="${@:3}"
pushd $fst > /dev/null
$snd setup.py $rst
popd > /dev/null
