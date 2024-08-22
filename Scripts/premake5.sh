#!/bin/bash

# Disable debug mode initially
set +x

# Determine the directory of the script
script_dir=$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")

# Check if the ORiginEngine environment variable is set
engine_path=$ORiginEngine
if [ -z "$engine_path" ]; then
    echo "Environment variable ORiginEngine is not set."
    exit 1
fi

pushd "$script_dir" > /dev/null

# Run the premake command with the chosen build system
"$engine_path/Scripts/premake/premake5" gmake2

# Return to the previous directory
popd > /dev/null

# Re-enable debug mode
set -x
