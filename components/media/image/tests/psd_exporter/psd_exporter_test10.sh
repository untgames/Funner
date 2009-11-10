#/bin/sh
echo Results of psd_exporter_test10:
psd-exporter data/test.psd --pot --crop-alpha=256 --crop-exclude='*2'
cat test.xlayermap
rm -rf test/* test  test.xlayermap
