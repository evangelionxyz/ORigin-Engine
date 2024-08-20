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

# Function to display the menu and get the user's choice
function show_menu {
    echo "Choose the build system:"
    echo "[1] CMake"
    echo "[2] GNU Make (gmake)"
    read -p "Enter your choice (1 or 2): " choice
}

# Show the menu until a valid choice is made
while true; do
    show_menu
    case $choice in
        1)
            build_system=cmake
            break
            ;;
        2)
            build_system=gmake2
            break
            ;;
        *)
            echo "Invalid choice. Please try again."
            ;;
    esac
done

# Navigate to the script directory
pushd "$script_dir" > /dev/null

# Run the premake command with the chosen build system
"$engine_path/Scripts/premake/premake5" $build_system

# Return to the previous directory
popd > /dev/null

# Re-enable debug mode
set -x
