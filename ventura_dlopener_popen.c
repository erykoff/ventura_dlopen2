#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLEN 1000


int open_dylib(char *dylib_name, int dylib_num) {
    char adder_name[MAXLEN], subtracter_name[MAXLEN];

    void *handle = dlopen(dylib_name, RTLD_NOW | RTLD_GLOBAL);

    if (!handle) {
        fprintf(stderr, "Error: unable to open %s\n", dylib_name);
        return -1;
    }

    snprintf(adder_name, MAXLEN, "adder%d", dylib_num);
    snprintf(subtracter_name, MAXLEN, "subtracter%d", dylib_num);

    double (*fa)() = dlsym(handle, adder_name);
    if (!fa) {
        fprintf(stderr, "Error: unable to get symbol %s from %s\n", adder_name, dylib_name);
        return -1;
    }
    double (*fs)() = dlsym(handle, subtracter_name);
    if (!fs) {
        fprintf(stderr, "Error: unable to get symbol %s from %s\n", subtracter_name, dylib_name);
        return -1;
    }

    //fprintf(stdout, "%s: %f\n", adder_name, fa(1.0, 2.0));
    //fprintf(stdout, "%s: %f\n", subtracter_name, fs(10.0, 4.0));

    return 0;
}

void load_all_dylibs(int procnum) {
    int ndylib = 1000;
    char *directory = "./dylibs";
    int retval = 0;
    char dylibname[MAXLEN];
    int i;

    fprintf(stderr, "Loading dylibs (%d)\n", procnum);

    for (i=0; i<ndylib; i++) {
        snprintf(dylibname, MAXLEN, "%s/source%d.dylib", directory, i);
        open_dylib(dylibname, i);
    }

    fprintf(stderr, "Done loading dylibs (%d)\n", procnum);
}

int main(int argc, char* argv[]) {
    int n;
    int nprocess = 20;
    int i;
    char cmd[MAXLEN];
    FILE *popen_files[100];
    int retval;

    if (argc == 2) {
        // Load the dylibs and exit
        n = strtol(argv[1], NULL, 0);

        load_all_dylibs(n);

        return 0;
    }

    // If no argument, spawn a bunch of processes with popen.

    for (i=0; i<nprocess; i++) {
        snprintf(cmd, MAXLEN, "./ventura_dlopener_popen %d", i);
        fprintf(stdout, "Running `%s`\n", cmd);
        popen_files[i] = popen(cmd, "r");
    }

    sleep(2);

    for (i=0; i<nprocess; i++) {
        retval = pclose(popen_files[i]);
        if (retval != 0) {
            fprintf(stderr, "Process %d crashed.\n", i);
        }
    }

    return 0;
}
