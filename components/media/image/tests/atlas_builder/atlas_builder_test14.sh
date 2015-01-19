#/bin/sh
echo Results of atlas_builder_test06:
mkdir -p results
atlas-builder data/1.bmp data/2.tga --margin=5 --atlas='results/edge-margin.png' --layout='results/edge-margin.xatlas' --top-right-edge-margin || exit 0
echo 'results/edge-margin.xatlas'
cat 'results/edge-margin.xatlas'
rm -rf results/* results
