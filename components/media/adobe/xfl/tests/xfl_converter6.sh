#/bin/sh
echo Results of xfl_converter_test6
mkdir -p results6
xfl-converter data/bench_animation.xfl --out-scene=results6/scene.xscene --crop-alpha=10 --loop-sprites="*"
cat results6/scene.xscene
cat results6/scene.xanim
rm -rf results6
