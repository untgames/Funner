#/bin/sh
echo Results of atlas_builder_test5:
mkdir results
atlas-builder data/1.bmp --atlas='results/swap-axises.png' data/2.tga data/1.bmp data/3.jpg --swap-axises || exit 0
echo 'results/swap-axises.xatlas'
cat 'results/swap-axises.xatlas'
