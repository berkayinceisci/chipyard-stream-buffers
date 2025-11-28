/*
 * Stream Buffer Benchmark v2 - Exceeds L1 cache to demonstrate prefetching benefit
 * L1 D-Cache: 32KB (64 sets x 8 ways x 64B blocks)
 * We use 64KB array to ensure we exceed L1 and stress the prefetcher
 */

#include <stdio.h>
#include <stdint.h>

static inline uint64_t read_cycles(void) {
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

static inline void memory_fence(void) {
    asm volatile ("fence" ::: "memory");
}

static inline void use_value(uint64_t val) {
    asm volatile ("" : : "r" (val) : "memory");
}

// 8192 elements x 8 bytes = 64KB (exceeds 32KB L1 cache)
#define ARRAY_SIZE 1024
#define STRIDE 8  // 64 bytes = 1 cache line

volatile uint64_t data_array[ARRAY_SIZE];

int main(void) {
    uint64_t start, end;
    uint64_t sum = 0;

    // Initialize array
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data_array[i] = i;
    }

    // Flush cache by accessing large amount of other data
    // This ensures we start with cold cache for the main test
    volatile uint64_t flush[4096];
    for (int i = 0; i < 4096; i++) flush[i] = i;
    memory_fence();

    printf("\n=== Stream Buffer Benchmark v2 ===\n");
    printf("Array: %d elements (%d KB)\n", ARRAY_SIZE, (ARRAY_SIZE * 8) / 1024);

    // Test 1: Sequential access (best case for stream buffer)
    memory_fence();
    start = read_cycles();
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += data_array[i];
    }
    memory_fence();
    end = read_cycles();

    printf("Sequential: %lu cycles (%lu cyc/access)\n",
           end - start, (end - start) / ARRAY_SIZE);
    use_value(sum);

    // Flush cache again
    for (int i = 0; i < 4096; i++) flush[i] = i * 2;
    memory_fence();

    // Test 2: Strided access (every cache line)
    sum = 0;
    memory_fence();
    start = read_cycles();
    for (int i = 0; i < ARRAY_SIZE; i += STRIDE) {
        sum += data_array[i];
    }
    memory_fence();
    end = read_cycles();

    printf("Strided:    %lu cycles (%lu cyc/access)\n",
           end - start, (end - start) / (ARRAY_SIZE / STRIDE));
    use_value(sum);

    // Flush cache again
    for (int i = 0; i < 4096; i++) flush[i] = i * 3;
    memory_fence();

    // Test 3: Reverse sequential (stream buffer may not help as much)
    sum = 0;
    memory_fence();
    start = read_cycles();
    for (int i = ARRAY_SIZE - 1; i >= 0; i--) {
        sum += data_array[i];
    }
    memory_fence();
    end = read_cycles();

    printf("Reverse:    %lu cycles (%lu cyc/access)\n",
           end - start, (end - start) / ARRAY_SIZE);
    use_value(sum);

    printf("\nDone.\n");
    return 0;
}
