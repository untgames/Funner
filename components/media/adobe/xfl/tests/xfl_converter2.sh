#/bin/sh
echo Results of xfl_converter_test2
mkdir -p results2
xfl-converter data/hall_vase_group_vase_pieces.xfl --out-scene=results2/scene.xml --crop-alpha=10
cat results2/scene.xml
rm -rf results2
