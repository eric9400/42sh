echo foo
echo $(
if true || false && true && false; then
    echo foo
else
    echo bar
fi)
