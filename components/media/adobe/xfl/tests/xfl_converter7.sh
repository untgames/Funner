#/bin/sh
echo Results of xfl_converter_test7
mkdir -p results7
xfl-converter data/bench_animation.xfl --out-scene=results7/scene.xml --crop-alpha=10 --loop-sprites="*" --total-scale="0.392;0.392" --total-offset="96;-37"
cat results7/scene.xml
rm -rf results7
