#/bin/sh
echo Results of xfl_converter_test1
mkdir -p results
xfl-converter data/ghost_left_group.xfl --out-scene=results/scene.xml --crop-alpha=10
cat results/scene.xml
