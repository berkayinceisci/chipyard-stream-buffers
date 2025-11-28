/*
 * Stream Buffer Prefetcher Microbenchmark
 *
 * This benchmark tests memory access patterns that should benefit from
 * stream buffer prefetching:
 * 1. Sequential read - traversing array sequentially (should benefit)
 * 2. Sequential write - writing array sequentially (should benefit)
 * 3. Strided access - accessing every N-th element (may benefit with stride detection)
 * 4. Random access - random array access (should NOT benefit - baseline)
 *
 * Compile: riscv64-unknown-elf-gcc -O2 -o stream_buffer_bench stream_buffer_bench.c
 */

#include <stdio.h>
#include <stdint.h>

// Read cycle counter
static inline uint64_t read_cycles(void) {
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}

// Read instruction counter
static inline uint64_t read_instret(void) {
    uint64_t instret;
    asm volatile ("rdinstret %0" : "=r" (instret));
    return instret;
}

// Prevent compiler from optimizing away the loop
static inline void use_value(uint64_t val) {
    asm volatile ("" : : "r" (val) : "memory");
}

// Memory fence to ensure all memory operations complete
static inline void memory_fence(void) {
    asm volatile ("fence" ::: "memory");
}

// Array size - make it large enough to exceed cache
// 64KB array = 8192 elements of 8 bytes each
// This should exceed typical L1 cache sizes (32KB)
#define ARRAY_SIZE 8192
#define ITERATIONS 4

// Global arrays to ensure they're in memory
volatile uint64_t data_array[ARRAY_SIZE];
volatile uint64_t result_array[ARRAY_SIZE];

// Simple LCG pseudo-random number generator
static uint32_t rand_state = 12345;
static inline uint32_t simple_rand(void) {
    rand_state = rand_state * 1103515245 + 12345;
    return (rand_state >> 16) & 0x7FFF;
}

// Test 1: Sequential Read
// This should benefit significantly from stream buffer prefetching
uint64_t test_sequential_read(void) {
    uint64_t start, end;
    uint64_t sum = 0;

    memory_fence();
    start = read_cycles();

    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            sum += data_array[i];
        }
    }

    memory_fence();
    end = read_cycles();

    use_value(sum);
    return end - start;
}

// Test 2: Sequential Write
// This should also benefit from stream buffer prefetching
uint64_t test_sequential_write(void) {
    uint64_t start, end;

    memory_fence();
    start = read_cycles();

    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            result_array[i] = i;
        }
    }

    memory_fence();
    end = read_cycles();

    return end - start;
}

// Test 3: Strided Read (stride = 8 elements = 64 bytes = 1 cache line)
// This tests stride detection capability
uint64_t test_strided_read(void) {
    uint64_t start, end;
    uint64_t sum = 0;
    const int stride = 8;

    memory_fence();
    start = read_cycles();

    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int i = 0; i < ARRAY_SIZE; i += stride) {
            sum += data_array[i];
        }
    }

    memory_fence();
    end = read_cycles();

    use_value(sum);
    return end - start;
}

// Test 4: Random Read
// This should NOT benefit from stream buffer prefetching
// Used as baseline to show the difference
uint64_t test_random_read(void) {
    uint64_t start, end;
    uint64_t sum = 0;

    // Generate random indices first
    static uint32_t indices[ARRAY_SIZE];
    rand_state = 12345; // Reset for reproducibility
    for (int i = 0; i < ARRAY_SIZE; i++) {
        indices[i] = simple_rand() % ARRAY_SIZE;
    }

    memory_fence();
    start = read_cycles();

    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            sum += data_array[indices[i]];
        }
    }

    memory_fence();
    end = read_cycles();

    use_value(sum);
    return end - start;
}

// Test 5: Pointer chasing (linked list traversal)
// Stream buffers typically can't help with this pattern
uint64_t test_pointer_chase(void) {
    uint64_t start, end;
    uint64_t sum = 0;

    // Create a shuffled linked list using the data array
    // Each element points to the next element in a pseudo-random order
    static uint32_t next[ARRAY_SIZE];
    rand_state = 54321;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        next[i] = (i + 1) % ARRAY_SIZE;
    }
    // Shuffle the next pointers
    for (int i = ARRAY_SIZE - 1; i > 0; i--) {
        int j = simple_rand() % (i + 1);
        uint32_t tmp = next[i];
        next[i] = next[j];
        next[j] = tmp;
    }

    memory_fence();
    start = read_cycles();

    int idx = 0;
    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            sum += data_array[idx];
            idx = next[idx];
        }
    }

    memory_fence();
    end = read_cycles();

    use_value(sum);
    return end - start;
}

int main(void) {
    uint64_t cycles_seq_read, cycles_seq_write, cycles_strided, cycles_random, cycles_chase;

    // Initialize data array
    for (int i = 0; i < ARRAY_SIZE; i++) {
        data_array[i] = i * 7 + 3;
    }

    // Warm up caches with a quick traversal
    volatile uint64_t warmup = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        warmup += data_array[i];
    }
    use_value(warmup);

    printf("\n=== Stream Buffer Prefetcher Benchmark ===\n");
    printf("Array size: %d elements (%d KB)\n", ARRAY_SIZE, (ARRAY_SIZE * 8) / 1024);
    printf("Iterations: %d\n\n", ITERATIONS);

    // Run benchmarks
    cycles_seq_read = test_sequential_read();
    cycles_seq_write = test_sequential_write();
    cycles_strided = test_strided_read();
    cycles_random = test_random_read();
    cycles_chase = test_pointer_chase();

    // Calculate cycles per element
    uint64_t total_accesses = (uint64_t)ARRAY_SIZE * ITERATIONS;
    uint64_t strided_accesses = ((uint64_t)ARRAY_SIZE / 8) * ITERATIONS;

    printf("Results (total cycles / cycles per access):\n");
    printf("  Sequential Read:  %lu cycles (%lu.%02lu cyc/elem)\n",
           cycles_seq_read,
           cycles_seq_read / total_accesses,
           (cycles_seq_read * 100 / total_accesses) % 100);

    printf("  Sequential Write: %lu cycles (%lu.%02lu cyc/elem)\n",
           cycles_seq_write,
           cycles_seq_write / total_accesses,
           (cycles_seq_write * 100 / total_accesses) % 100);

    printf("  Strided Read:     %lu cycles (%lu.%02lu cyc/elem)\n",
           cycles_strided,
           cycles_strided / strided_accesses,
           (cycles_strided * 100 / strided_accesses) % 100);

    printf("  Random Read:      %lu cycles (%lu.%02lu cyc/elem)\n",
           cycles_random,
           cycles_random / total_accesses,
           (cycles_random * 100 / total_accesses) % 100);

    printf("  Pointer Chase:    %lu cycles (%lu.%02lu cyc/elem)\n",
           cycles_chase,
           cycles_chase / total_accesses,
           (cycles_chase * 100 / total_accesses) % 100);

    printf("\n=== Analysis ===\n");
    printf("Sequential vs Random speedup: %lu.%02lux\n",
           cycles_random / cycles_seq_read,
           (cycles_random * 100 / cycles_seq_read) % 100);

    printf("\nBenchmark complete.\n");

    return 0;
}
