#!/bin/bash

mkdir test
cp -r code test

cat code/14.c

n=0
while(($n < 16)) 
do
	gcc -c test/code/$n.c -o test/code/$n.o
	((n++))	
done

gcc test/code/*.o -o test/hello

./test/hello 2> test/err.txt

mv test/err.txt ./

chmod u+rw,g+r,o+xr,a+x err.txt

n1=1
n2=1

if [ $# -eq 0 ]; then
	n0=$((n1+n2))
	sed -n "${n0}p" err.txt >&2
elif [ $# -eq 1 ]; then
	n1=$1
	n0=$((n1+n2))
	sed -n "${n0}p" err.txt >&2
else
	n1=$1
	n2=$2
	n0=$((n1+n2))
	sed -n "${n0}p" err.txt >&2
fi
