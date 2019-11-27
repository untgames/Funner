#/bin/sh
echo Results of atlas_builder_test15:
mkdir -p results
atlas-builder data/1.bmp data/2.tga --alignment=1 --atlas='results/alignment1.png' --layout='results/alignment1.xatlas' || exit 0
echo 'results/alignment1.xatlas'
cat 'results/alignment1.xatlas'
atlas-builder data/1.bmp data/2.tga --alignment=3 --atlas='results/alignment3.png' --layout='results/alignment3.xatlas' || exit 0
echo 'results/alignment3.xatlas'
cat 'results/alignment3.xatlas'
atlas-builder data/1.bmp data/2.tga --alignment=4 --atlas='results/alignment4.png' --layout='results/alignment4.xatlas' || exit 0
echo 'results/alignment4.xatlas'
cat 'results/alignment4.xatlas'
atlas-builder data/1.bmp data/2.tga --alignment=5 --atlas='results/alignment5.png' --layout='results/alignment5.xatlas' || exit 0
echo 'results/alignment5.xatlas'
cat 'results/alignment5.xatlas'
atlas-builder data/1.bmp data/2.tga --margin=2 --alignment=1 --atlas='results/margin2alignment1.png' --layout='results/margin2alignment1.xatlas' || exit 0
echo 'results/margin2alignment1.xatlas'
cat 'results/margin2alignment1.xatlas'
atlas-builder data/1.bmp data/2.tga --margin=2 --alignment=4 --atlas='results/margin2alignment4.png' --layout='results/margin2alignment4.xatlas' || exit 0
echo 'results/margin2alignment4.xatlas'
cat 'results/margin2alignment4.xatlas'
rm -rf results/* results
