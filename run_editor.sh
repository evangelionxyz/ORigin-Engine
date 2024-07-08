#!/bin/bash
export LD_LIBRARY_PATH=$ORiginEngine/Binaries/Debug/ORigin:$LD_LIBRARY_PATH

pushd ./Binaries/Debug/ORigin > /dev/null

/home/evangelion/Dev/ORigin-Engine/Binaries/Debug/ORigin/Editor

popd > /dev/null
