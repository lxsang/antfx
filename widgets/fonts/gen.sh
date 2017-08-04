#!/bin/sh
for file in Free*.h; do
    num=`grep -o 0x $file | wc -l`
    num=`expr $num - 95`
    num=`expr $num - 2`
    echo "$num" >> $file
done