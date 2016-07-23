#/bin/sh
echo Results of psd_exporter_test07:
psd-exporter data/test.psd --silent --layers-dir=my_layers --layout-layers-dir=test --layers-format=my_image_%04u.bmp -o my.xlayermap
cat my.xlayermap
for file in my_layers/*; do md5sum $file; done
rm -rf my_layers/* my_layers  my.xlayermap
