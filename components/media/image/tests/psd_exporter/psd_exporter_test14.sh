#/bin/sh
echo Results of psd_exporter_test14:
psd-exporter data/zcolor.psd --zcolor-min="0 0 0" --zalpha=0 --layers-dir=my_layers --layers-format=my_image_%04u.png -o my.xlayermap --crop-alpha=1
cat my.xlayermap
for file in my_layers/*; do md5sum -b $file; done
rm -rf my_layers/* my_layers  my.xlayermap
