#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXSTRLEN 1000
#define MAXDYLIBS 2000


int open_dylib(char *dylib_name, int dylib_num) {
    // Open a dylib and find the adder and subtracter symbols.
    // Each dylib has unique symbol names with dylib_num.

    char adder_name[MAXSTRLEN], subtracter_name[MAXSTRLEN];

    void *handle = dlopen(dylib_name, RTLD_NOW | RTLD_GLOBAL);

    if (!handle) {
        fprintf(stderr, "Error: unable to open %s\n", dylib_name);
        return -1;
    }

    snprintf(adder_name, MAXSTRLEN, "adder%d", dylib_num);
    snprintf(subtracter_name, MAXSTRLEN, "subtracter%d", dylib_num);

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

    // For debugging, make sure the functions are properly loaded.
    //fprintf(stdout, "%s: %f\n", adder_name, fa(1.0, 2.0));
    //fprintf(stdout, "%s: %f\n", subtracter_name, fs(10.0, 4.0));

    return 0;
}

void load_all_dylibs(int ndylib, int procnum) {
    char *directory = "./dylibs";
    int retval = 0;
    char dylibname[MAXSTRLEN];
    int i;

    fprintf(stderr, "Loading %d dylibs (%d)\n", ndylib, procnum);

    for (i=0; i<ndylib; i++) {
        snprintf(dylibname, MAXSTRLEN, "%s/source%d.dylib", directory, i);
        open_dylib(dylibname, i);
    }

    fprintf(stderr, "Done loading %d dylibs (%d)\n", ndylib, procnum);
}

int main(int argc, char* argv[]) {
    int n;
    int ndylibs;
    int nprocess = 20;
    int i;
    char cmd[MAXSTRLEN];
    FILE *popen_files[100];
    int retval;

    if (argc < 2) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  ventura_dlopener_popen N_DYLIBS INDEX (to load lots of dylibs in one process).\n");
        fprintf(stderr, "  ventura_dlopener_popen N_DYLIBS (to load lots of dylibs in 20 processes).\n");
        fprintf(stderr, "Maximum number of dylibs is %d.\n", MAXDYLIBS);
        return -1;
    }

    if (argc == 3) {
        // Load the dylibs and exit.
        ndylibs = strtol(argv[1], NULL, 0);
        n = strtol(argv[2], NULL, 0);

        if (ndylibs > MAXDYLIBS) {
            fprintf(stderr, "Must be <= %d.\n", MAXDYLIBS);
            return -1;
        }

        load_all_dylibs(ndylibs, n);

        return 0;
    } else if (argc == 2) {
        // Get number of dylibs from command line.
        ndylibs = strtol(argv[1], NULL, 0);

        if (ndylibs > MAXDYLIBS) {
            fprintf(stderr, "Must be <= %d.\n", MAXDYLIBS);
            return -1;
        }
    }

    // Spawn a bunch of processes to each load ndylibs libraries.

    for (i=0; i<nprocess; i++) {
        snprintf(cmd, MAXSTRLEN, "./ventura_dlopener_popen %d %d", ndylibs, i);
        fprintf(stdout, "Running `%s`\n", cmd);
        popen_files[i] = popen(cmd, "r");
    }

    // This makes sure the processes spawn correctly before we try to close them.
    sleep(2);

    // Close up all the processes, and check that they ran okay.
    for (i=0; i<nprocess; i++) {
        retval = pclose(popen_files[i]);
        if (retval != 0) {
            fprintf(stderr, "Process %d crashed.\n", i);
        } else {
            fprintf(stdout, "Process %d succeeded!\n", i);
        }
    }

    return 0;
}
