/*
 * Stream Buffer Benchmark v3 - Demonstrates prefetching on COLD cache
 *
 * Key insight: Stream buffers help with COMPULSORY misses (first access to data)
 * They prefetch the NEXT cache lines before they're needed.
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

// Large enough to exceed L1 cache significantly
// 8192 elements x 8 bytes = 64KB (2x L1 cache)
#define ARRAY_SIZE 8192

// Two separate arrays - we alternate to ensure cold cache
volatile uint64_t array_a[ARRAY_SIZE];
volatile uint64_t array_b[ARRAY_SIZE];

// Access one element per cache line (64 bytes / 8 bytes = 8 elements per line)
// This maximizes cache misses and prefetch opportunities
#define CACHE_LINE_ELEMS 8

int main(void) {
    uint64_t start, end;
    uint64_t sum = 0;
    int num_lines = ARRAY_SIZE / CACHE_LINE_ELEMS;

    // Initialize arrays
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array_a[i] = i;
        array_b[i] = i * 2;
    }
    memory_fence();

    printf("\n=== Stream Buffer Benchmark v3 (Cold Cache) ===\n");
    printf("Array: %d elements (%d KB), %d cache lines\n",
           ARRAY_SIZE, (ARRAY_SIZE * 8) / 1024, num_lines);

    // Test 1: Touch array_b to evict array_a from cache
    sum = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) sum += array_b[i];
    use_value(sum);
    memory_fence();

    // Now access array_a with COLD cache - one element per cache line
    sum = 0;
    memory_fence();
    start = read_cycles();

    for (int i = 0; i < ARRAY_SIZE; i += CACHE_LINE_ELEMS) {
        sum += array_a[i];  // Access first element of each cache line
    }

    memory_fence();
    end = read_cycles();

    printf("Cold Sequential (1/line): %lu cycles (%lu cyc/line)\n",
           end - start, (end - start) / num_lines);
    use_value(sum);

    // Test 2: Now array_a should be warm, access array_b (cold)
    memory_fence();
    sum = 0;
    start = read_cycles();

    for (int i = 0; i < ARRAY_SIZE; i += CACHE_LINE_ELEMS) {
        sum += array_b[i];
    }

    memory_fence();
    end = read_cycles();

    printf("Cold Sequential (1/line): %lu cycles (%lu cyc/line)\n",
           end - start, (end - start) / num_lines);
    use_value(sum);

    // Test 3: Warm cache access (array_b should now be cached)
    memory_fence();
    sum = 0;
    start = read_cycles();

    for (int i = 0; i < ARRAY_SIZE; i += CACHE_LINE_ELEMS) {
        sum += array_b[i];
    }

    memory_fence();
    end = read_cycles();

    printf("Warm Sequential (1/line): %lu cycles (%lu cyc/line)\n",
           end - start, (end - start) / num_lines);
    use_value(sum);

    printf("\nExpected: Cold >> Warm, Stream buffer helps Cold\n");
    printf("Done.\n");
    return 0;
}
