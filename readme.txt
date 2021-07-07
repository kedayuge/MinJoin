1. compile:  
git clone https://github.com/kedayuge/MinJoin.git
cd MinJoin
g++ edit.cpp timerec.cpp hash.cpp minjoin.cpp partition.cpp -std=c++11 -o minjoin -O3

2. usage: ./embedjoin inputdata K T q dictsize

inputdata: a set of strings in a file

K: edit distance threshold

T: Number of targeted partitions, should be O(K), e.g. [K/5, K]

q: length of q-gram, e.g. [2, 10] (depends on alpha beta size of input strings)

dictsize: alpha beta size of input strings, could be 5 for DNA dataset (ACGTN); 26 for UNIREF dataset (A~Z); 37 for TREC dataset (A~Z,0~9,' ') 

3. example input:
./minjoin dnadata50k.txt 150 50 10 5
./minjoin uniref.txt 25 20 2 26
./minjoin trec.txt 50 10 2 37 

Note that, all output pairs are distinct, and verified by exact edit distance filter. 
Thus, the number of output divided by number of ground truth (by exact algorithm such as Passjoin, Qchunck) is the accuracy.

4.Input format:
A collection of strings seperated by enter
For example:
ACGTTTTTTTAATTTTATAGGGGGC
ACGTAATTTAATTTTATAGGGGGC
ACGTTTTTTTATTTATAGGGTTGGC
...

Data download:
https://indiana-my.sharepoint.com/:f:/g/personal/hz30_iu_edu/EnlnpMqC6K1BooxeFc-zzIMBX0UtB63bGZ7Z2Revtb_rqQ?e=IRK1ms
