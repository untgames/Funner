#/bin/sh
echo Results of launcher_test02:
license-generator -c "*" -p 0-01 -f "launcher02.sh" -o launcher02.license
clauncher --config=launcher02_config.xml --no-main-loop "lua: print('hello')"
rm -f launcher02.license