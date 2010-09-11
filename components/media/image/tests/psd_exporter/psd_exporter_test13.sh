#/bin/sh
echo Results of psd_exporter_test13:
psd-exporter data/zero_alpha.psd --layers-dir=my_layers --layers-format=my_image_%04u.bmp -o my.xlayermap --fix-zero-alpha=5 --blur-passes-count=10
cat my.xlayermap
for file in my_layers/*; do md5sum $file; done
rm -rf my_layers/* my_layers  my.xlayermap
