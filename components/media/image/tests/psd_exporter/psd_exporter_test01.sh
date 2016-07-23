#/bin/sh
echo Results of psd_exporter_test01:
psd-exporter data/test.psd
cat test.xlayermap
for file in test/*; do md5sum $file; done
rm -rf test/* test test.xlayermap
