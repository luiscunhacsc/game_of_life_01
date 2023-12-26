#include <stdio.h>

// function to see if a number is prime
int isPrime(int n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    int n = 1000000;
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (isPrime(i)) count++;
    }
    printf("Found %d primes\n", count);
    return 0;
}