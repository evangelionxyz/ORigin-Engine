#!/usr/bin/env bash
set +x
script_dir=$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")
pushd "$script_dir" > /dev/null
"premake/premake5" gmake2
popd > /dev/null
set -x
