#/bin/sh
echo Results of psd_exporter_test09:
psd-exporter data/crop.psd --crop-alpha=20
cat crop.xlayermap
for file in crop/*; do md5sum $file; done
rm -rf crop/* crop  crop.xlayermap
