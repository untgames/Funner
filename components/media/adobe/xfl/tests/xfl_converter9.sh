#/bin/sh
echo Results of xfl_converter_test9
mkdir -p results9
xfl-converter data/frame_with_offset.xfl --out-scene=results9/scene.xscene
cat results9/scene.xscene
cat results9/scene.xanim
rm -rf results9
