#/bin/sh
echo Results of atlas_builder_test4:
mkdir results
atlas-builder --atlas='results/complex.png' --max-image-size=256 data/1.bmp data/2.tga data/4.png data/3.jpg data/4.png || exit 0
echo 'results/complex0.xatlas'
cat 'results/complex0.xatlas'
echo 'results/complex1.xatlas'
cat 'results/complex1.xatlas'
echo 'results/complex2.xatlas'
cat 'results/complex2.xatlas'
