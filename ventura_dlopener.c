#include <stdio.h>
#include <dlfcn.h>
#include <pthread.h>


int open_dylib(char *dylib_name, int dylib_num) {
    char adder_name[1000], subtracter_name[1000];

    void *handle = dlopen(dylib_name, RTLD_NOW | RTLD_GLOBAL);

    if (!handle) {
        fprintf(stderr, "Error: unable to open %s\n", dylib_name);
        return -1;
    }

    snprintf(adder_name, 1000, "adder%d", dylib_num);
    snprintf(subtracter_name, 1000, "subtracter%d", dylib_num);

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

    // fprintf(stdout, "%s: %f\n", adder_name, fa(1.0, 2.0));
    // fprintf(stdout, "%s: %f\n", subtracter_name, fs(10.0, 4.0));

    return 0;
}

void* load_all_dylibs_in_thread(void *p) {
    int ndylib = 500;
    char *directory = "./dylibs";
    int retval = 0;
    char dylibname[1000];
    int i;

    fprintf(stdout, "Starting thread number %d\n", *(int*)p);

    for (i=0; i<ndylib; i++) {
        snprintf(dylibname, 1000, "%s/source%d.dylib", directory, i);
        open_dylib(dylibname, i);
    }

    pthread_exit(&retval);
}


int main(int argc, char* argv[]) {
    int nthread = 20;
    int *retptr;
    int i;

    pthread_t id[100];
    int thread_nums[100];

    for (i=0; i<nthread; i++) {
        thread_nums[i] = i;
    }

    fprintf(stdout, "Starting threads...\n");
    for (i=0; i<nthread; i++) {
        pthread_create(&id[i], NULL, load_all_dylibs_in_thread, &thread_nums[i]);
    }

    fprintf(stdout, "Joining threads...\n");
    for (i=0; i<nthread; i++) {
        pthread_join(id[i], (void**)&retptr);

        fprintf(stdout, "Thread %d done.\n", i);
    }

    return 0;
}
