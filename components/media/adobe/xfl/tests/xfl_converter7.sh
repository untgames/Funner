#/bin/sh
echo Results of xfl_converter_test7
mkdir -p results7
xfl-converter data/bench_animation.xfl --out-scene=results7/scene.xscene --crop-alpha=10 --loop-sprites="*" --total-scale="0.392;0.392" --total-offset="96;-37"
cat results7/scene.xscene
cat results7/scene.xanim
rm -rf results7
