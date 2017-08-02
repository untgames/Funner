#/bin/sh
echo Results of psd_exporter_test16:
psd-exporter data/test.psd --silent --layers-dir=my_layers --layers-format=my_image_%04u.bmp -o my.xlayermap --skip-layers="*4"
cat my.xlayermap
for file in my_layers/*; do md5sum $file; done
rm -rf my_layers/* my_layers  my.xlayermap
