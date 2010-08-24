#/bin/sh
echo Results of xfl_converter_test2
mkdir -p results
xfl-converter data/hall_vase_group_vase_pieces.xfl --out-scene=results/scene.xml --crop-alpha=10
cat results/scene.xml
