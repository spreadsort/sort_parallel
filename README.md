
<h1>Boost Sort Parallel</h1>
<h3> <a href="https://github.com/fjtapia/sort_parallel">https://github.com/fjtapia/sort_parallel</a> </h3>

<h3>PRELIMINARY</h3>

The **Boost Sort Parallel Library**, had been designed for to be included in the Boost Sort Library, created by Steven Ross, which actually have the SpreadSort algorithms.

This library is pending of the final approval for to be included in the Boost Sort Libray, due this, can suffer some changes until the final version and definitive approval in the Boost Library.

<h3>DESCRIPTION</h3>

This library provide **stable and not stable** sorting algorithms, in **single thread and parallel** versions.

These algorithms do not use any other library or utility. For to **compile and run** only need a **C++11 compliant compiler.**

The algorithms **use an comparison object**, in the same way than the sort algorithms of the standard library. If you don't define it, by default is std::less object, using internally the operator < for to do the comparison.

The algorithms are **exception safe**, it means,  the exceptions generated by the algorithms guarantee the integrity of the objects to sort , but not their relative order. If the exception is generated inside the objects (in the move or in the copy constructor.. ) the results can be unpredictable.

**This library is include only**. Don't need to link with any static or dynamic library. Don't have  dependence of any other boost files, variables or libraries or any other external libraries. For to use, only need a to include the files of the boost/sort/parallel folder, any more.

This table provide you a brief description of the sort algorithms of the library.

| Algorithm | Parallel | Stable | Additional memory | Best, average, and worst case |
| --- | --- | --- | --- | --- |
| sort | no | no | Log N  | NLogN, NLogN, NLogN |
| stable\_sort | no | yes| N / 2 | NLogN, NLogN, NLogN   |
| parallel\_sort | yes | no | 1024\*number_threads | NLogN, NLogN, NLogN  |
| parallel\_stable\_sort| yes | yes | N / 2 | NLogN, NLogN, NLogN   |
| sample\_sort | yes | yes | N  | NLogN, NLogN, NLogN  |

<h3>PRESENT PERSPECTIVE</h3>


In the parallel sorting algorithms, we can find two categories of algorithms.

**SUBDIVISION ALGORITHMS**

Filter the data and generate two or more parts. Each part obtained is filtered and divided by other thread, until the size of the data to sort is smaller than a predefined size, then it is sorted by a single thread. The algorithm most frecuently used in the filter and sorting is quicksort.

These algorithms are  **fast with a small number of threads**, but with a great number of HW threads , show their lacks. Examples of this category are 

 - Intel Threading Building Blocks (TBB) 
 - Microsoft PPL Parallel Sort.


**MERGING ALGORITHMS**

Divide the data in parts, and each part is sorted by a thread. When the parts are sorted, must merge them for obtain the final results. The problem of these algorithms is they need additional memory for the merge, and usually with the same size than the data.

With a small number of threads, have similar speed than the subdivision algorithms, but **with many threads they are  much more faster** . Examples of this category are :

 - GCC Parallel Sort (based on OpenMP)
 - Microsoft PPL Parallel Buffered Sort

<h3>NEW PARALLEL SORT ALGORITHM</h3>
This generate an **undesirable duality**. With a small number of threads use one algorithm, and with a big number use other. Due this, the SW designed for a small machine is inadequate for a big machine and vice versa. But the main problem, for the merging algorithms is the memory used, usually of the same size than the data.

This version have as novelty a **new parallel\_sort algorithm** *( internally named Block Indirect)*, created for processors connected with **shared memory**.

It is a hybrid algorithm. With small number of threads, it is a subdivision algorithm, but with many threads is a merging algorithms, which need a small  auxiliary memory ( 1024 * number of threads).

This algorithm permit to eliminate the duality. When run in a machine with small number of threads have the performance of TBB, and when run in a machine with many threads, the same code have the performance of GCC Parallel Sort, with the **additional advantage** of the **small memory consumption**.

The algorithm use as **auxiliary memory a 1024 elements** for each thread. The worst case for the algorithm is when have very big elements and many threads. With big elements (512 bytes), and 32 threads, The memory measured was:

| Algorithm | Memory used in MB |
| --- | --- |
| GCC Parallel Sort (OpenMP) | 1565 MB |
| Threading Building Blocks (TBB) | 783 MB |
| Block Indirect Sort | 812 MB |

*This **new parallel\_sort algorithm had been  created and implemented specifically for this library** by the author.*

*You can obtain more information in the Documentation Page of the Project  * [index.html](index.html)

*If you  are interested in a brief description of the algorithm, you can find in the next link* 
[(block_indirect_sort_brief_en.pdf)](block_indirect_sort_brief_en.pdf)

*If you  are interested in a detailed description of the algorithm, you can find in the next link* 
[(block_indirect_sort_en.pdf)](block_indirect_sort_en.pdf)

*If you want run the benchmarks in your machine, you have all the code, intructions and procedures in*
<a href="https://github.com/fjtapia/sort_parallel_benchmark">https://github.com/fjtapia/sort_parallel_benchmark</a>

<h2>Installation </h2>
 * This library is **include only**.  
 * Don't need to link with any static or dynamic library.
 * Don't have  dependence of any other boost libraries or any other external libraries.
 * For to use, only need include the files of the boost/sort/parallel folder.  
 * This library had been compiled successfully with the next compilers
 
| Compiler |
| --- |
| GCC  4.8, 4.9 , 5.2 |
| CLANG 3.6, 3.7|
| Visual C++ 2013 ,2015|


<h2>Author and Copyright</h2>
This library had been create for to be integrated in the [Boost](http://www.boost.org) library, inside the [boost::sort library](http://www.boost.org/doc/libs/release/libs/sort),
with the spreadsort algorithms designed and implemented by Steven Ross.

It's pending of the final approval, due this can suffer some changes until the final version and definitive approval in the boost library.  You can find in  [https://github.com/fjtapia/sort_parallel](https://github.com/fjtapia/sort_parallel)

This algorithm had been ideate, designed and implemented beginning from zero. After read hundreds of articles and books, I didn't find any similar. If someone knows something about this or something similar, please, say me.

Anyway, the important is not the author, is provide a fast, robust, and easy to use algorithm to the community of programmers.

Copyright 2016  [Francisco Tapia *(fjtapia@gmail.com)* ](mail:fjtapia@gmail.com)
Distributed under the [Boost Software License, Version 1.0. ](http://www.boost.org/LICENSE_1_0.txt)  (See http://www.boost.org/LICENSE_1_0.txt)
