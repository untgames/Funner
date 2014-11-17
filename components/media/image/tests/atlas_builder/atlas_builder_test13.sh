#/bin/sh
echo Results of atlas_builder_test06:
mkdir -p results
atlas-builder data/1.bmp data/2.tga --margin=25 --atlas='results/pot-edge-margin.png' --layout='results/pot-edge-margin.xatlas' --pot --top-right-edge-margin || exit 0
echo 'results/pot-edge-margin.xatlas'
cat 'results/pot-edge-margin.xatlas'
