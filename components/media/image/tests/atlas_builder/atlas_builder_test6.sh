#/bin/sh
echo Results of atlas_builder_test5:
mkdir results
atlas-builder data/1.bmp --atlas='results/pot-duplicate.png' data/2.tga data/2.tga --pot || exit 0
echo 'results/pot-silent.xatlas'
cat 'results/pot-silent.xatlas'
