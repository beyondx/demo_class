#!/bin/bash


SRC=test.c
DST=demo.txt

for  i in {1..1000} ;do
	cat $SRC >> $DST
done

ls -lh $SRC $DST
