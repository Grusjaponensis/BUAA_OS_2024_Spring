#!/bin/bash
B_file=$2
A_file=$1

touch {$B_file}
sed -n '8p;32p;128p;512p;1024p;' $A_file > $B_file 

