#include <windows.h>
#include <stdio.h>
#include <intrin.h>

int main()
{
    int* target_data = (int*)malloc(sizeof(int));
    *target_data = 40;
    
    volatile int dummy; // volatile forces the compiler not to optimize this away
    unsigned long long start, end;
    unsigned long long cache_miss_cycles, cache_hit_cycles;
    
    // Command the hardware to physically evict this address from L1, L2, and L3 cache
    _mm_clflush(target_data); 

    // CPU STOP: Wait for the flush to finish
    _mm_lfence();         
    start = __rdtsc();
    // CPU STOP: Don't read the memory until the timer starts
    _mm_lfence();

    // THE ACTUAL READ (Guaranteed to fetch from slow RAM)
    dummy = *target_data;

    // CPU STOP: Wait for the read to finish
    _mm_lfence();
    // Stop Timer
    end = __rdtsc();      
    cache_miss_cycles = end - start;

    
    // Because we just read 'target_data', it is now sitting safely inside the L1 Cache.
    _mm_lfence();
    start = __rdtsc(); 
    _mm_lfence();

    // THE ACTUAL READ (Guaranteed to fetch from fast L1)
    dummy = *target_data;

    _mm_lfence();
    end = __rdtsc();
    cache_hit_cycles = end - start;

    printf("Cache miss latency: %llu cycles\n", cache_miss_cycles);
    printf("Cache hit latency:  %llu cycles\n", cache_hit_cycles);

    free(target_data);
    return 0;
}