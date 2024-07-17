#!/usr/bin/sh

# this must correspond to the project name configured in the CMakeLists.txt
PROJECT_NAME=mc_clone

# ---

cmake -B cmake -DCMAKE_BUILD_TYPE=Debug

cd cmake

make

cp bin/$PROJECT_NAME ../bin

cp lib/* ../lib

cd ..

cp -r src/shaders bin

cp -r src/assets bin

cd bin

./$PROJECT_NAME

cd ..
