#!/usr/bin/env python3

import os


nfiles = 1000

for i in range(nfiles):
    fname = f"source{i}.c"

    if os.path.isfile(fname):
        continue

    with open(fname, "w") as f:
        f.write("#include <stdio.h>\n")
        f.write("\n")
        f.write(f"double adder{i}(double a, double b)"" {\n")
        f.write("    return(a + b);\n")
        f.write("}\n")
        f.write("\n")
        f.write(f"double subtracter{i}(double a, double b)"" {\n")
        f.write("    return(a - b);\n")
        f.write("}\n")

