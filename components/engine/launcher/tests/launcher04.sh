#/bin/sh
echo Results of launcher_test04:
license-generator -c "*" -p 0-01 -f "launcher04.sh" -o launcher04.license
clauncher --config=launcher04_config.xml --no-main-loop launcher04.lua
rm -f launcher04.license
