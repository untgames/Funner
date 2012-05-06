#/bin/sh
echo Results of ttf2xfont_test07:
mkdir -p results
ttf2xfont data/a_damn_mess.ttf --image=results/a_damn_mess.png --out-font=results/a_damn_mess.xfont --glyph-size=32 --first-glyph-code=99 --last-glyph-code=100 --silent
cat results/a_damn_mess.xfont
md5sum -b results/a_damn_mess.png
rm -rf results

