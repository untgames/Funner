#!/bin/sh
set -x
set -e

rm -rf dependencies
mkdir dependencies
git clone https://github.com/EsotericSoftware/spine-runtimes.git dependencies
cd dependencies
git config core.autocrlf input
git reset --hard
git rm .gitattributes
git checkout .
git checkout 54c20904fda6d352277ac703ddb7c81d275eaaa6
git apply --reject --whitespace=fix ../funner.patch
mv spine-c/spine-c/include ../include
rm -rf ../sources
mv spine-c/spine-c/src ../sources
cd ..
rm -rf dependencies
