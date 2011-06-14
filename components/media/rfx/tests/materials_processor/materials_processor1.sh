#/bin/sh
echo Results of materials_processor_test1
mkdir -p results
materials-processor data/source.xmtl -o results/result.xmtl -m "data/merge.xmtl"
cat results/result.xmtl