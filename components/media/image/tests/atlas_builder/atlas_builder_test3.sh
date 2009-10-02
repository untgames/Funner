#/bin/sh
echo Results of atlas_builder_test3:
atlas-builder data/1.bmp --atlas='results/atlas1.png' data/2.tga && atlas-builder data/3.jpg -o 'results/atlas2.png' --layout='results/atlas2-custom.xatlas' && atlas-builder -o results/my' image'.png data/1.bmp -l 'results/my.xatlas' || exit 0
echo 'results/atlas1.xatlas'
cat 'results/atlas1.xatlas'
echo 'results/atlas2-custom.xatlas'
cat 'results/atlas2-custom.xatlas'
echo 'results/my.xatlas'
cat 'results/my.xatlas'
