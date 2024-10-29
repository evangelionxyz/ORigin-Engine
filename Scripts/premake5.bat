#!/usr/bin/env bash
set +x

# Get the directory of the current script
script_dir=$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")

engine_path=$ORiginEngine
if [ -z "$engine_path" ]; then
    echo "environment variable ORiginEngine is not set."
    echo "exiting..."
    exit 1
fi

# Push the script directory to the stack
pushd "$script_dir" > /dev/null

# Run the premake5 command
"$premake55" gmake2

# Pop the directory from the stack
popd > /dev/null

set -x