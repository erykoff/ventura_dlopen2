# ventura_dlopen2

Making ventura crash some more.
Number of dylibs to crash the dynamic loader appears to depend on the resources of the machine.

Go into dylibs and run `bash compile_dylibs.sh`.
This will compile 2000 dylibs which seems to be more than enough to crash Ventura even on an M1 Ultra with 64Gb of ram.
(You can also make more dylibs with `python3 generate_dylib_files.py` if necessary.)

Then in the parent directory run `bash build_opener.sh`.

Then run `./ventura_dlopener_popen 500` or `./ventura_dlopener_popen 1000` (up to `./ventura_dlopener_popen 2000`) to run 20 processes loading that number of dylibs.
If you just want to run in one process, you can do `./ventura_dlopener_popen 2000 0`.
The threshold on an M1 seems to be around 931; the threshold on an M1 Ultra is somewhere between 1000 and 1500.
