#/bin/sh
echo Results of atlas_builder_test5:
mkdir results
atlas-builder data/1.bmp --atlas='results/pot-duplicate.png' --layout='results/pot-duplicate.xatlas' data/2.tga data/2.tga --pot || exit 0
echo 'results/pot-duplicate.xatlas'
cat 'results/pot-duplicate.xatlas'
