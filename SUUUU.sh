#/bin/sh

if [ $# -ne 0 ] && [ $# -ne 1 ]; then
    echo "problemo in el pasta"
    echo "Usage: "$0" [clean]"
    exit 1
fi

if [ $# -eq 1 ]; then
    if [[ $1 == "clean" ]]; then
        rm -rf "builddir"
        rm -rf "httpd"
        echo "clean done"
        exit 0
    fi
    echo "Usage: "$0" [clean]"
    exit 1
fi

rm -rf "builddir"
rm -rf "httpd"

# compile 
# >=======
meson setup builddir
echo "meson setup build done"

ninja -C build
echo "ninja done"
# =======<

# cp binary in current branch
cp "builddir/httpd" "."
echo "build finish"

exit 0
