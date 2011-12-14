#/bin/sh
echo Results of xfl_converter_test4
mkdir -p results4
xfl-converter data/bench_animation.xfl --out-scene=results4/scene.xml --crop-alpha=10
cat results4/scene.xml
rm -rf results4
