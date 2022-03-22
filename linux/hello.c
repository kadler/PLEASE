#include <stdio.h>

int main(int argc, char** argv) {
    int rc = 42;

    if (argc < 2) {
        printf("Hello from a Linux binary!\n");
    }
    else {
        printf("Hello from a Linux binary, %s!\n", argv[1]);
    }

    printf("Returning %d\n", rc);
    return rc;
}
