#/bin/sh
echo Results of launcher_test08:
license-generator -c "*" -p 0-01 -f "launcher08.sh" -o launcher08.license
clauncher --config=launcher08_config.xml utilities.lua launcher08.lua
rm -f launcher08.license
