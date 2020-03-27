#/bin/sh
echo Results of atlas_builder_test16:
mkdir -p results
atlas-builder data/1.bmp data/2.tga --margin=2 --bottom-left-edge-margin --atlas='results/margin1.png' --layout='results/margin1.xatlas' || exit 0
echo 'results/margin1.xatlas'
cat 'results/margin1.xatlas'
rm -rf results/* results
