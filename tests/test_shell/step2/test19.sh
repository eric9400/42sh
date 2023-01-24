while false; do
	echo coucou;
	if true || false; then
		echo "\\\\''bar";
	fi
done
var=true
for i in 1 2 3 4 5
do
	if $var; then
		var=false
	else
		var=true;
	fi
done