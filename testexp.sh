#!/bin/bash
for i in `seq 1 10000`;
do
./exp 127.0.0.3 readme.txt &
done
