#/bin/sh
echo Results of atlas_builder_test05:
mkdir -p results
atlas-builder data/1.bmp --atlas='results/pot-silent.png' --layout='results/pot-silent.xatlas' data/2.tga --pot --silent || exit 0
echo 'results/pot-silent.xatlas'
cat 'results/pot-silent.xatlas'
