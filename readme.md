## General Infomation
**What language have you written your solution in?**
I used C++.
To build the project, use
```
g++ -std=c++11 ./src/suffix_array.cpp ./src/querysa.cpp -o ./bin/querysa
g++ -std=c++11 ./src/suffix_array.cpp ./src/buildsa.cpp -o ./bin/buildsa
 ```
**What resources did you consult in working on this assignment?**
Lecture notes and the suffix array implementation I wrote long time ago.

## Report Items
### Part A
**What did you find to be the most challenging part of implementing the buildsa program?**
I used the prefix doubling algorithm to construct the suffix array, which is simpler than the DC3 algorithm. However, the manipulation of arrays and indices was still tedious and a minor mistake would result in errors.
**For references of various size: how long does building the suffix array take?**
I concatenated all sequences in the FASTA file to build a long reference for testing. The total length of the sequences is 2,047,601. The construct time grows as the length of the references increases. Furthermore, it also takes more time to build the prefix table since the keys in the hash table become longer as k increases.
##### Table: Construction time (unit: ms)
|n| k=0| k=4|k=8|k=12|k=16|
|---|---|---|---|---|---|
|1*10^6^|378|604|678|1167|1382|
|2*10^6^|944|1553|1648|2674|3234|

**How large is the resulting serialized file?**
I only store the reference and the suffix array, and re-construct the prefix table after deserializing the suffix array. Consequently, the size is n(s~c~+s~i~), where n is the length of the reference, s~c~ is the size of a char, and s~i~ is the size of an integer. For example, the size of the index is 4 times greater than the reference if indices in the suffix array are stored as 32bit integers.
**For the times and sizes above, how do they change if you use the --preftab option with some different values of k?**
The storage cost does not change because I re-build the prefix table but it needs more memory for the prefix table as k increases. I only created entries for those prefix existing in the reference, so the space cost is n(k*s~c~+2s~i~)+n(s~c~+s~i~).
**Given the scaling above, how large of a genome do you think you could construct the suffix array for on a machine with 32GB of RAM, why?**
Without the prefix table, the memory cost is also n(s~c~+s~i~), and each extra character in the reference needs 40 bits if the indices are stored in the form of 32-bit integers. In this case, the maximum length of the reference is 6.87 * 10^9^ if we want to fit the index in 32GB of RAM. With the prefix table, the space cost of each extra character is (k+1)s~c~+3s~i~, and it is 200 bits per character so the maximum length becomes 1.37 * 10^9^. In my implementation, I also build an auxiliary array to store the rank of the suffixes and create the prefix table, but it is not necessary for the query algorithms.

### Part B
**What did you find to be the most challenging part of implementing the query program?**
The query algorithms are simpler than the construction, but the efficiency in practice is still influenced by details in implementation.
**For references of various size: How long does query take on references of different size, and on queries of different length?**
A set of 1*10^6^ queries are generated for each pair of n and k. The query time of the naive search algorithm grows as the length of queries increases, but not linearly as predicted by the theoretical time complexity, O(m log n). It also takes a little more time when the reference gets longer.
##### Table: Naive query time when k=0 (unit: ms)
|n | m=20| m=40|m=80|m=160|m=320|
|---|---|---|---|---|---|
|1*10^6^|2947|3174|3550|4375|5851|
|2*10^6^|3351|3484|3802|4616|6103|
**How does the speed of the naive lookup algorithm compare to the speed of the simpleaccel lookup algorithm?**
The query time is reduced when the simpleaccel algorithm is used. The actual speedup is from 1.15 to 1.39 depending on the different values of n and m, comparing to the naive algorithm.
##### Table: Simpleaccel query time when k=0 (unit: ms)
|n | m=20| m=40|m=80|m=160|m=320|
|---|---|---|---|---|---|
|1*10^6^|2162|2304|2678|3540|5039|
|2*10^6^|2405|2537|2843|3621|5205|
**How does the speed further compare when not using a prefix lookup table versus using a prefix lookup table with different values of k?**
The query time dereases as k grows, but the improvement is limited when k becomes larger (the difference between k=12 and k=16 is very small), while it needs more space to store the prefix table.
##### Table: Simpleaccel query time by varying k (unit: ms)
|n |k|Alg.| m=20| m=40|m=80|m=160|m=320|
|---|---|---|---|---|---|---|---|
|1*10^6^|0|Naive|2947|3174|3550|4375|5851|
|1*10^6^|0|Accel|2162|2304|2678|3540|5039|
|2*10^6^|0|Naive|3351|3484|3802|4616|6103|
|2*10^6^|0|Accel|2405|2537|2843|3621|5205|
|1*10^6^|4|Naive|2582|2792|3177|3941|5484|
|1*10^6^|4|Accel|1922|2117|2511|3276|4793|
|2*10^6^|4|Naive|2930|3117|3411|4371|5750|
|2*10^6^|4|Accel|2205|2348|2655|3436|5143|
|1*10^6^|8|Naive|1772|2014|2382|3184|4721|
|1*10^6^|8|Accel|1587|1788|2200|2951|4496|
|2*10^6^|8|Naive|2351|2692|2676|3414|4973|
|2*10^6^|8|Accel|1965|2265|2431|3118|4711|
|1*10^6^|12|Naive|1307|1509|1904|2679|4173|
|1*10^6^|12|Accel|1315|1534|1910|2648|4176|
|2*10^6^|12|Naive|1580|1682|1996|2773|4267|
|2*10^6^|12|Accel|1571|1707|1994|2740|4261|
|1*10^6^|16|Naive|1335|1518|1919|2665|4171|
|1*10^6^|16|Accel|1313|1509|1906|2652|4198|
|2*10^6^|16|Naive|1527|1679|1977|2697|4216|
|2*10^6^|16|Accel|1546|1677|1969|2726|4200|
**Given the scaling above, and the memory requirements of each type of index, what kind of tradeoff do you personally think makes sense in terms of using more memory in exchange for faster search.**
Greater values of k provide better search efficiency, but also lead to higher memory cost. Moreover, the speedup is limited as k increases so it is a waste of space if k is too large. One may choose a proper value of k so that the index fits into the memory. For example, k=12 would be appropriate for the reference and queries above.