#!/usr/bin/env bash

for ((i=0;i<1000;i++));
do
    echo $i
    clang -c source${i}.c -o source${i}.o
    clang -dynamiclib -flat_namespace source${i}.o -o source${i}.dylib
done
