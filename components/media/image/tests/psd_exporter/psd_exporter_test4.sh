#/bin/sh
echo Results of psd_exporter_test4:
psd-exporter data/test.psd --silent --layers-dir=my_layers --layers-format=my_image_%04u.tga -o my.xlayermap
cat my.xlayermap
rm -rf my_layers/* my_layers  my.xlayermap
