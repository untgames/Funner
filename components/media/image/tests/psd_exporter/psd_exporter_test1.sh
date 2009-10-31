#/bin/sh
echo Results of psd_exporter_test1:
psd-exporter data/test.psd
cat test.xlayermap
rm -rf test/* test test.xlayermap