#/bin/sh
echo Results of launcher_test09:
license-generator -c "*" -p 0-01 -f "launcher09.sh" -o launcher09.license
clauncher --config=launcher09_config.xml utilities.lua launcher09.lua
rm -f launcher09.license
