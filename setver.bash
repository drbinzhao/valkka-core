#!/bin/bash
if [ $# -ne 3 ]; then
  echo "Give major minor patch"
  exit
fi
# mod header files:
find include/ -name "*.h" -exec sed -i -r "s/@version (.*)/@version $1.$2.$3 /g" {} \;
#
# mod python package
sed -i -r "s/version = (.*)/version = \"$1.$2.$3\", /g" python/setup.py
#
# mod cmake (for making debian packages) 
sed -i -r "s/SET\(MAJOR_VERSION (.*)/SET(MAJOR_VERSION \"$1\") /g" CMakeLists.txt
sed -i -r "s/SET\(MINOR_VERSION (.*)/SET(MINOR_VERSION \"$2\") /g" CMakeLists.txt
sed -i -r "s/SET\(PATCH_VERSION (.*)/SET(PATCH_VERSION \"$3\") /g" CMakeLists.txt
#
# mod doxygen documentation
sed -i -r "s/PROJECT_NUMBER(.*)/PROJECT_NUMBER = \"$1.$2.$3\"/g" doc/config
#
# mod version numbers in sizes.h
sed -i -r "s/VERSION_MAJOR(.*);/VERSION_MAJOR = $1;/g" include/sizes.h
sed -i -r "s/VERSION_MINOR(.*);/VERSION_MINOR = $2;/g" include/sizes.h
sed -i -r "s/VERSION_PATCH(.*);/VERSION_PATCH = $3;/g" include/sizes.h