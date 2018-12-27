#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <time.h>

void delay(int number_of_seconds);
int main() {
    int i;
    for (i = 0; i < 10; i++) {
        delay(1);
        printf("%d seconds have passed\n", i + 1);
    }

    return 0;

}

void delay(int number_of_seconds) {
    // Converting time into milli_seconds
    int milli_seconds = CLOCKS_PER_SEC * number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}

void delay_2() {
    int c, d;

    for (c = 1; c <= 32767; c++)
        for (d = 1; d <= 32767; d++) {}

    printf("This C program will exit in 10 seconds.\n");

}