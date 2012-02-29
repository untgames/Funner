#/bin/sh
echo Results of xfl_converter_test5
mkdir -p results5
xfl-converter data/bench_animation.xfl --out-scene=results5/scene.xscene --ignore-image-size --crop-alpha=10
cat results5/scene.xscene
cat results5/scene.xanim
rm -rf results5
