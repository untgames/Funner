#/bin/sh
echo Results of ttf2xfont_test06:
mkdir -p results
ttf2xfont data/a_damn_mess.ttf --image=results/a_damn_mess.png --out-font=results/a_damn_mess.xfont --glyph-size=32 --glyph-interval=20 --force-advance=50 --silent
cat results/a_damn_mess.xfont
md5sum results/a_damn_mess.png
rm -rf results1

