#/bin/sh
echo Results of xfl_converter_test8
mkdir -p results8
xfl-converter data/negative_position.xfl --out-scene=results8/scene.xscene
cat results8/scene.xscene
cat results8/scene.xanim
rm -rf results8
