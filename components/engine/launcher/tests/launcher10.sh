#/bin/sh
echo Results of launcher_test10:
license-generator -c "*" -p 0-01 -f "launcher10.sh" -o launcher10.license
clauncher --config=launcher10_config.xml --no-main-loop launcher10.lua
rm -f launcher10.license
