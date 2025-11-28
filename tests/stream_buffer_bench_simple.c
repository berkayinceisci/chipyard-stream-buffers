/*
 * Simple Stream Buffer Benchmark - designed for fast RTL simulation
 */

#include <stdio.h>
#include <stdint.h>

// Read cycle counter
static inline uint64_t read_cycles(void) {
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

static inline void memory_fence(void) {
    asm volatile ("fence" ::: "memory");
}

// Small array for faster simulation
#define ARRAY_SIZE 256
#define ITERATIONS 2

volatile uint64_t data_array[ARRAY_SIZE];

int main(void) {
    uint64_t start, end;
    uint64_t sum = 0;

    // Initialize
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data_array[i] = i * 7 + 3;
    }

    printf("\n=== Simple Stream Buffer Benchmark ===\n");
    printf("Array: %d elements, %d iterations\n\n", ARRAY_SIZE, ITERATIONS);

    // Test: Sequential Read
    memory_fence();
    start = read_cycles();
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            sum += data_array[i];
        }
    }
    memory_fence();
    end = read_cycles();

    uint64_t seq_cycles = end - start;
    uint64_t total_accesses = ARRAY_SIZE * ITERATIONS;

    printf("Sequential Read: %lu cycles (%lu cyc/access)\n",
           seq_cycles, seq_cycles / total_accesses);

    // Prevent optimization
    if (sum == 0) printf("sum=%lu\n", sum);

    printf("\nBenchmark complete.\n");
    return 0;
}
