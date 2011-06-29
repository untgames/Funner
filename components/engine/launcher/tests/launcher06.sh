#/bin/sh
echo Results of launcher_test06:
license-generator -c "*" -p 0-01 -f "launcher06.sh" -o launcher06.license
clauncher --config=launcher06_config.xml launcher06.lua
rm -f launcher06.license
