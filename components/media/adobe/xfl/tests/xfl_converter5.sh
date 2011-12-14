#/bin/sh
echo Results of xfl_converter_test5
mkdir -p results5
xfl-converter data/bench_animation.xfl --out-scene=results5/scene.xml --ignore-image-size --crop-alpha=10
cat results5/scene.xml
rm -rf results5
