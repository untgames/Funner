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
cp -R examples/alien ../../../../components/media/spine/tests/data/
cp -R examples/coin ../../../../components/media/spine/tests/data/
cp -R examples/dragon ../../../../components/media/spine/tests/data/
cp -R examples/goblins ../../../../components/media/spine/tests/data/
cp -R examples/tank ../../../../components/media/spine/tests/data/
cp -R examples/vine ../../../../components/media/spine/tests/data/
cp -R examples/vine ../../../../components/scene_graph/tests/controllers/data/
cp -R examples/vine ../../../../components/scene_graph/tests/core/data/
cp -R examples/alien ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/coin ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/dragon ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/goblins ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/hero ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/powerup ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/raptor ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/speedy ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/spineboy ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/stretchyman ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/tank ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/vine ../../../../components/render/scene_render/server/tests/renderer/data/
cp -R examples/goblins ../../../../components/engine/script_binds/tests/lua/data/
cd ..
rm -rf dependencies
