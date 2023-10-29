### Task

Make generator of random elements in random order without duplicates from vector of elements. Original container can be mutated if necessary. Only overall time is important: does not matter whether algorithm pre-shuffles elements or calculates next element when called.


Solutions are slightly simplified so that they modify preallocated vectors instead of returning elements one by one.

### Considered approaches

1. std::shuffle whole container and return values with incrementing index.
2. Naive approach: store visited indices, choose random index, check if visited, if not, visit and return.
3. Fisher-Yates shuffle: can calculate elements one by one (unlike std::shuffle, that does not have such API).

> What about sampling algorithms?

While they are fast, order of elements usually depends on the order of elements in the original container.

### Considered RNGs

1. PCG
2. mt19937

### Results

Hardware:
```
CPU: i5-13600k 
RAM: 32Gb 3200MHz
```

Running in WSL2 (may impact performance).

```
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
BM_MT19937                 101819792 ns    101818871 ns            7
BM_PCG                      51635794 ns     51635277 ns           13
BM_PCG_NEXT_ONE/0           24169268 ns     24165407 ns           30
BM_PCG_NEXT_ONE/1           61894440 ns     61894258 ns           12
BM_PCG_NEXT_ONE/2          112298414 ns    112296740 ns            5
BM_PCG_NEXT_ONE/3          411679688 ns    411610550 ns            2
BM_PCG_NEXT/0               19183223 ns     19183044 ns           36
BM_PCG_NEXT/1               52722297 ns     52721869 ns           13
BM_PCG_NEXT/2              100670335 ns    100557914 ns            7
BM_PCG_NEXT/3              337299065 ns    337300150 ns            2
BM_PCG_FISHER_YATES/0       11410791 ns     11410682 ns           62
BM_PCG_FISHER_YATES/1       20676429 ns     20676192 ns           36
BM_PCG_FISHER_YATES/2       34823320 ns     34823305 ns           20
BM_PCG_FISHER_YATES/3       51845168 ns     51816762 ns           13
BM_PCG_FISHER_YATES_UNI/0   10556601 ns     10556468 ns           63
BM_PCG_FISHER_YATES_UNI/1   20867161 ns     20823994 ns           36
BM_PCG_FISHER_YATES_UNI/2   34394956 ns     34394595 ns           21
BM_PCG_FISHER_YATES_UNI/3   54965894 ns     54943550 ns           12
```

### Conclusions

- Shuffling with PCG is ~2x faster than MT19937.
- Naive approach returning elements one by one will seemingly be slower than when returning elements in batches due to memory access pattern. This probably applies to other approaches allowing returning values one by one.
- Naive approach expectedly slows down a lot when number of random elements gets closer to the size of the original container.
- Naive approach slows down when accessing 2 vectors (probably worse cache behaviour): if we write indices instead of values into the `res` vector, it is much faster (x2 on 0.25 elements).
- Fisher-Yates seems to be the same solution used in std::shuffle (judging by performance).
- Uniform distribution is almost as fast as modulo (modulo is slightly biased).
- Fisher-Yates with uniform distribution and PCG is the best solution.