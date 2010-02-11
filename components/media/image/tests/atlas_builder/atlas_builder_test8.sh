#/bin/sh
echo Results of atlas_builder_test5:
mkdir results
atlas-builder --atlas='results/swap-axises.png' --layout='results/swap-axises.xatlas' data/1.bmp data/3.jpg data/2.tga --swap-axises --pot || exit 0
echo 'results/swap-axises.xatlas'
cat 'results/swap-axises.xatlas'
