#include <stdio.h>

int sum(int* buffer, int len);

int main(void) {
    const int length = 10;
    int buffer[length];
    int buffer_sum = 0;
    for(int i = 0; i < length; ++i) {
        buffer[i] = i;
    }
    buffer_sum = sum(buffer, length);
    printf("buffer_sum = %d\n", buffer_sum); 
    return 0;
}