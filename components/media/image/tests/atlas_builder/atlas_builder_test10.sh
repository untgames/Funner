#/bin/sh
echo Results of atlas_builder_test10:
mkdir -p results
atlas-builder data/1.bmp --atlas='results/pot-margin.png' --layout='results/pot-margin.xatlas' data/2.tga --pot --margin=10 || exit 0
echo 'results/pot-margin.xatlas'
cat 'results/pot-margin.xatlas'
