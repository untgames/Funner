#/bin/sh
echo Results of atlas_builder_test09:
mkdir -p results
atlas-builder --atlas='results/complex.png' --layout='results/complex.xatlas' --max-image-size=256 data/1.bmp data/2.tga data/4.png data/3.jpg data/4.png || exit 0
echo 'results/complex.xatlas'
cat 'results/complex.xatlas'
rm -rf results/* results
