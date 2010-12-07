#/bin/sh
echo Results of atlas_builder_test11:
mkdir -p results
atlas-builder data/one_pixel.bmp --atlas='results/one_pixel.png' --layout='results/one_pixel.xatlas' --pot || exit 0
echo 'results/one_pixel.xatlas'
cat 'results/one_pixel.xatlas'
