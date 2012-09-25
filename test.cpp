#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "scene.h"

int main() {
    int i = 0;
    Scene scene;

    srandom(time(NULL));
    printf("\nbegin add object=============\n");
    for (i = 0; i < 10; ++i) {
        scene.Add(i + 1, random() % 50, random() % 50);
    }

    printf("\nbegin move object=============\n");
    for (i = 0; i < 100; ++i) {
        int x = random() % 50;
        int y = random() % 50;
        scene.Move(i % 10, x, y);
    }

    printf("\nbegin leave object=============\n");
    for (i = 0; i < 10; ++i) {
        scene.Leave(i + 1);
    }

    return 0;
}
