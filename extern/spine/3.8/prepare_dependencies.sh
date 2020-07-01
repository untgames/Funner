#!/bin/sh
set -x
set -e

rm -rf dependencies
mkdir dependencies
cd dependencies
git init
git config core.autocrlf input
git remote add origin https://github.com/EsotericSoftware/spine-runtimes.git
git fetch --depth 1 origin b67bf992a0349b779aac9f619f027d34e296267c
git checkout FETCH_HEAD
git reset --hard
git rm .gitattributes
git checkout .
git apply --reject --whitespace=fix ../funner.patch
mv spine-c/spine-c/include ../include
rm -rf ../sources
mv spine-c/spine-c/src ../sources
mkdir ../../../../components/media/spine/tests/data/3.8
cp -R examples/alien ../../../../components/media/spine/tests/data/3.8/
cp -R examples/coin ../../../../components/media/spine/tests/data/3.8/
cp -R examples/dragon ../../../../components/media/spine/tests/data/3.8/
cp -R examples/goblins ../../../../components/media/spine/tests/data/3.8/
cp -R examples/tank ../../../../components/media/spine/tests/data/3.8/
cp -R examples/vine ../../../../components/media/spine/tests/data/3.8/
mkdir ../../../../components/scene_graph/tests/controllers/data/spine_3.8/
cp -R examples/vine ../../../../components/scene_graph/tests/controllers/data/spine_3.8/
mkdir ../../../../components/scene_graph/tests/core/data/spine_3.8/
cp -R examples/vine ../../../../components/scene_graph/tests/core/data/spine_3.8/
mkdir ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/alien ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/coin ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/dragon ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/goblins ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/hero ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/powerup ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/raptor ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/speedy ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/spineboy ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/stretchyman ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/tank ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
cp -R examples/vine ../../../../components/render/scene_render/server/tests/renderer/data/spine_3.8/
mkdir ../../../../components/engine/script_binds/tests/lua/data/spine_3.8/
cp -R examples/goblins ../../../../components/engine/script_binds/tests/lua/data/spine_3.8/
cd ..
rm -rf dependencies
