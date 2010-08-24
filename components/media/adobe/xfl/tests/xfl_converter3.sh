#/bin/sh
echo Results of xfl_converter_test3
mkdir -p results
xfl-converter data/frog_frame_group_final.xfl --out-scene=results/scene.xml --crop-alpha=10
cat results/scene.xml
