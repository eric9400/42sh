for n in 1 2 3;
do
    echo \$n;
    if true; then
        continue
    fi
done
