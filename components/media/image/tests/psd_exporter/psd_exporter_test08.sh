#/bin/sh
echo Results of psd_exporter_test08:
psd-exporter data/test.psd --silent --pot
cat test.xlayermap
for file in test/*; do md5sum -b $file; done
rm -rf test/* test  test.xlayermap
