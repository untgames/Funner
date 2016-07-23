#/bin/sh
echo Results of psd_exporter_test11:
psd-exporter data/test.psd --trim-names --silent --layers-dir=my_layers --layers-format=my_image_%04u.bmp -o my.xlayermap --max-image-size=64
cat my.xlayermap
for file in my_layers/*; do md5sum $file; done
rm -rf my_layers/* my_layers  my.xlayermap
