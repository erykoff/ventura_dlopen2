#!/usr/bin/env bash

clang -O0 ventura_dlopener_popen.c -o ventura_dlopener_popen
clang -O0 ventura_dlopener_fuzz.c -o ventura_dlopener_fuzz
