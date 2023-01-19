# ventura_dlopen2

Making ventura crash some more.

Go into dylibs and run `compile_dylibs.sh`.
You can also make more with `generate_dylib_files.py`.

Then in the parent directory run `bash build_opener.sh`.

Then run `./ventura_dlopener_popen 500` or `./ventura_dlopener_popen 1000` to run 20 processes loading that number of dylibs.
If you need a different number of processes edit the code and recompile.
