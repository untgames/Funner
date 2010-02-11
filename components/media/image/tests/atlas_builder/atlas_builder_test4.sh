#/bin/sh
echo Results of atlas_builder_test4:
mkdir results
atlas-builder data/1.bmp --atlas='results/pot.png' --layout='results/pot.xatlas' data/2.tga --pot || exit 0
echo 'results/pot.xatlas'
cat 'results/pot.xatlas'
