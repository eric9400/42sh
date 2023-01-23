for i in toto foo bar hello; do
    echo $i
    for j in toto2 bar2; do
        echo $j
        continue
        echo "not suppose to be printed"
    done
    echo second loop done
done
