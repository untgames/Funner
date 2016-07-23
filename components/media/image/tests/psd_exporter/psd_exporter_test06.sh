#/bin/sh
echo Results of psd_exporter_test06:
psd-exporter data/test.psd --silent -O my --no-layout
ls my/*
for file in my/*; do md5sum $file; done
rm -rf my/* my
