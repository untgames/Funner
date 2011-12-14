#/bin/sh
echo Results of launcher_test07:
license-generator -c "*" -p 0-01 -f "launcher07.sh" -o launcher07.license
clauncher --config=launcher07_config.xml launcher07.lua
rm -f launcher07.license
