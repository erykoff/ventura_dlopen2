#!/usr/bin/env bash

clang -O0 ventura_dlopener.c -o ventura_dlopener
clang -O0 ventura_dlopener_fork.c -o ventura_dlopener_fork
clang -O0 ventura_dlopener_popen.c -o ventura_dlopener_popen
