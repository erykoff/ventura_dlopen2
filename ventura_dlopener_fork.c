#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/wait.h>


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

    //fprintf(stdout, "%s: %f\n", adder_name, fa(1.0, 2.0));
    //fprintf(stdout, "%s: %f\n", subtracter_name, fs(10.0, 4.0));

    return 0;
}

void load_all_dylibs(int i) {
    int ndylib = 500;
    char *directory = "./dylibs";
    int retval = 0;
    char dylibname[1000];

    fprintf(stdout, "Starting process number %d\n", i);

    for (i=0; i<ndylib; i++) {
        snprintf(dylibname, 1000, "%s/source%d.dylib", directory, i);
        open_dylib(dylibname, i);
    }
}

int main(int argc, char* argv[]) {
    int nprocess = 10;
    int i;

    pid_t pids[100];
    int process_nums[100];
    pid_t pid;
    int child_status;

    for (i=0; i<nprocess; i++) {
        process_nums[i] = i;
    }

    fprintf(stdout, "Starting processes...\n");
    for (i=0; i<nprocess; i++) {
        pid = fork();

        if (pid == 0) {
            // This is a child process
            fprintf(stdout, "Child %d has been created.\n", process_nums[i]);
            load_all_dylibs(process_nums[i]);
        } else if (pid == -1) {
            fprintf(stderr, "An error forking occurred.\n");
            return -1;
        } else {
            // This is the parent process
            pids[i] = pid;
        }
    }

    fprintf(stdout, "Waiting for children...\n");
    for (i=0; i<nprocess; i++) {
        pid = waitpid(pids[i], &child_status, 0);
        fprintf(stdout, "Child %d has been joined.\n", i);
    }

    return 0;
}
