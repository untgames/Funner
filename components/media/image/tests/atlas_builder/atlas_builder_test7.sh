#/bin/sh
echo Results of atlas_builder_test5:
mkdir results
atlas-builder data/1.bmp --atlas='results/invert-x.png' data/2.tga --invert-x && atlas-builder data/1.bmp --atlas='results/invert-y.png' data/2.tga --invert-y && atlas-builder data/1.bmp --atlas='results/invert-xy.png' data/2.tga --invert-x --invert-y || exit 0
echo 'results/invert-x.xatlas'
cat 'results/invert-x.xatlas'
echo 'results/invert-y.xatlas'
cat 'results/invert-y.xatlas'
echo 'results/invert-xy.xatlas'
cat 'results/invert-xy.xatlas'
