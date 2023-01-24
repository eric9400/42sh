var=true
for i in 1 2 3 4 5
do
	if $var; then
		var=false
	else
		var=true;
	fi
	while false; do
		echo coucou;
		if true || false; then
			echo "\\\\''bar";
		fi
	done;
	for k in 1 2 3 4 5; do
		echo "$var + $i + $k";
	done;
done