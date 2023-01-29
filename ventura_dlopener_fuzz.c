#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>


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

void shuffle(int *array, size_t n) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);

    if (n > 1) {
        size_t i;
        for (i = n - 1; i > 0; i--) {
            size_t j = (unsigned int) (drand48()*(i+1));
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void load_all_dylibs(int ndylib) {
    char *directory = "./dylibs";
    int retval = 0;
    char dylibname[MAXSTRLEN];
    int i, j;
    int arr[MAXDYLIBS];

    fprintf(stderr, "Loading %d dylibs\n", ndylib);

    for (i=0; i<ndylib; i++) {
        arr[i] = i;
    }

    shuffle(arr, ndylib);

    for (i=0; i<ndylib; i++) {
        j = arr[i];
        snprintf(dylibname, MAXSTRLEN, "%s/source%d.dylib", directory, j);
        fprintf(stderr, "Opening index %d, number %d\n", i, j);
        open_dylib(dylibname, j);
        // do it again to triangulate possible crashes.
        open_dylib(dylibname, j);
    }

    fprintf(stderr, "Done loading %d dylibs\n", ndylib);
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
        fprintf(stderr, "  ventura_dlopener_fuzz N_DYLIBS (to load lots of dylibs in one process in random order).\n");
        fprintf(stderr, "Maximum number of dylibs is %d.\n", MAXDYLIBS);
        return -1;
    }

    // Load the dylibs and exit.
    ndylibs = strtol(argv[1], NULL, 0);

    if (ndylibs > MAXDYLIBS) {
        fprintf(stderr, "Must be <= %d.\n", MAXDYLIBS);
        return -1;
    }

    load_all_dylibs(ndylibs);

    return 0;
}
