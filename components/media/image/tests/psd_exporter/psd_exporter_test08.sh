#/bin/sh
echo Results of psd_exporter_test08:
psd-exporter data/test.psd --silent --pot
cat test.xlayermap
rm -rf test/* test  test.xlayermap
