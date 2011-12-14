#/bin/sh
echo Results of xfl_converter_test3
mkdir -p results3
xfl-converter data/frog_frame_group_final.xfl --out-scene=results3/scene.xml --crop-alpha=10
cat results3/scene.xml
rm -rf results3
