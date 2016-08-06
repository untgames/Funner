#/bin/sh
echo Results of launcher_test05:
license-generator -c "*" -p 0-01 -f "launcher05.sh" -o launcher05.license
clauncher --config=launcher05_config.xml --no-main-loop launcher05.lua
rm -f launcher05.license
