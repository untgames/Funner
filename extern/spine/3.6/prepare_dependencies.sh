#!/bin/sh
rm -rf dependencies
mkdir dependencies
git clone https://github.com/EsotericSoftware/spine-runtimes.git dependencies
cd dependencies
git checkout 54c20904fda6d352277ac703ddb7c81d275eaaa6
git apply ../funner.patch
mv spine-c/spine-c/include ../include
rm -rf ../sources
mv spine-c/spine-c/src ../sources
cd ..
rm -rf dependencies