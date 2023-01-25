
if true || false && false || false && true; then
	a=5;
	echo foo bar $a;
	b=$a;
	a=$a
	a=6
	echo $b $a;
else
	echo error;
fi
