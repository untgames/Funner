#/bin/sh
echo Results of texture-converter_test02:
mkdir -p results
texture-converter data/bottom_atc_rgb.dds --source-format=atitc_rgb --target='results/bottom_atc_rgb.png' && \
texture-converter data/selection_atc_rgba_explicit_alpha.dds --source-format=atitc_rgba_explicit_alpha --target='results/selection_atc_rgba_explicit_alpha.png' && \
texture-converter data/selection_atc_rgba_interpolated_alpha.dds --source-format=atitc_rgba_interpolated_alpha --target='results/selection_atc_rgba_interpolated_alpha.png' && \
texture-converter data/npot.dds --source-format=atitc_rgba_explicit_alpha --target='results/npot.png' && \
texture-converter data/npot_non_square.dds --source-format=atitc_rgba_explicit_alpha --target='results/npot_non_square.png' && \
texture-converter results/bottom_atc_rgb.png --target-format=atitc_rgb --target='results/bottom_atc_rgb.dds' && \
texture-converter results/selection_atc_rgba_explicit_alpha.png --target-format=atitc_rgba_explicit_alpha --target='results/selection_atc_rgba_explicit_alpha.dds' && \
texture-converter results/selection_atc_rgba_interpolated_alpha.png --target-format=atitc_rgba_interpolated_alpha --target='results/selection_atc_rgba_interpolated_alpha.dds' && \
texture-converter results/npot.png --target-format=atitc_rgba_explicit_alpha --target='results/npot.dds' && \
texture-converter results/npot_non_square.png --target-format=atitc_rgba_explicit_alpha --target='results/npot_non_square.dds' || exit 0
for file in results/*; do md5sum -b $file; done
rm -rf results/* results
