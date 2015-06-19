rm uncrustify.lst 2> /dev/null
find . -type f \( -name "*.c" -o -name "*.h" \) -not -path "./external/*" > uncrustify.lst
bin/macosx/uncrustify -c scripts/uncrustify.cfg --no-backup -F uncrustify.lst
rm uncrustify.lst 2> /dev/null

