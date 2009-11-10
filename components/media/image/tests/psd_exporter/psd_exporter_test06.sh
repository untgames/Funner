#/bin/sh
echo Results of psd_exporter_test06:
psd-exporter data/test.psd --silent -O my --no-layout
ls my/*
rm -rf my/* my
