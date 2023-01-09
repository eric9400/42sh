#/bin/sh

dir=builddir
bin=42sh

if [ $# -ne 0 ] && [ $# -ne 1 ]; then
    echo "problemo in el pasta"
    echo "Usage: "$0" [clean]"
    exit 1
fi

if [ $# -eq 1 ]; then
    if [[ $1 == "clean" ]]; then
        rm -rf "$dir"
        rm -rf "$bin"
        echo "clean done"
        exit 0
    fi
    echo "Usage: "$0" [clean]"
    exit 1
fi

rm -rf "$dir"
rm -rf "$bin"

# compile 
# >=======
meson setup "$dir"
echo "meson setup build done"

ninja -C "$dir"
echo "ninja done"
# =======<

# cp binary in current branch
cp "$dir"/"$bin" "."
echo "build finish"

exit 0
