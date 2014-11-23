#/bin/sh
echo Results of xfl_converter_test4
mkdir -p results4
xfl-converter data/bench_animation.xfl --out-scene=results4/scene.xscene --crop-alpha=10
cat results4/scene.xscene
cat results4/scene.xanim
rm -rf results4
