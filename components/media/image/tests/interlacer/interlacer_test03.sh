#/bin/sh
echo Results of interlacer_test03:
mkdir -p results
interlacer data/image001.png data/image002.png data/image003.png data/image004.png -o results/image%03d.png
for file in results/*; do md5sum $file; done
rm -Rf results
