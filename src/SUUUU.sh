#!/bin/sh

dir=builddir
bin=42sh
testsuite=test_shell

if [ $# -ne 0 ] && [ $# -ne 1 ]; then
    echo "problemo in el pasta"
    echo "Usage: "$0" [clean]"
    exit 1
fi

if [ $# -eq 1 ]; then
    if [ $1 = "clean" ]; then
        rm -rf "$dir"
        rm -rf "$bin"
        rm -f "a"
        rm -rf "$testsuite"
        echo "clean done"
        exit 0
    elif [ $1 = "debug" ]; then
        debug="-Db_sanitize=address"
    else
        echo "Usage: "$0" [clean | debug]"
    exit 1
    fi
fi

rm -rf "$dir"
rm -rf "$bin"

# build
# >=======
meson setup $dir $debug
echo "meson setup build done"
# =======<

# compile
# >=======
ninja -C "$dir"
echo "ninja done"
# =======<
#
# cp binary in current branch
cp "$dir"/"$bin" "."
echo "build finish"

# if [ "$1" = "debug" ]; then
#     cp -r "../tests/""$testsuite" "."
#     ./test_shell/test_suite.sh
# fi

exit 0
