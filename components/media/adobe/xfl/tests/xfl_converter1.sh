#/bin/sh
echo Results of xfl_converter_test1
mkdir -p results1
xfl-converter data/ghost_left_group.xfl --out-scene=results1/scene.xscene --crop-alpha=10
cat results1/scene.xscene
cat results1/scene.xanim
rm -rf results1
