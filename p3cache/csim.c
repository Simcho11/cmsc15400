#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
typedef struct cacheLine {
  int valid;
  int lru;
  long tag;
} cacheLine;
cacheLine** cache; //main cache matrix
struct trace {
    char operation;
    long memory;
    int tsize;
} trace;
//helpers to parse traces
int getOffset(int blockBits, long address) {
    int mask = (1 << blockBits) - 1;;
    //printf("offset mask: %d\n",mask);
    return address & mask;
}
int getSet(int blockBits, int indexBits, long address) {
    address = address >> blockBits;
    int mask = (1 << indexBits) - 1;
    //printf("set mask: %d\n",mask);
    return address & mask;
}
long getTag(int blockBits, int indexBits, long address) {
    return address >> (indexBits + blockBits);
}
void printTrace(struct trace t) {
    //helper that prints traces for verbose
    char op = t.operation;
    long mem = t.memory;
    int s = t.tsize;
    printf(" %c %lx,%d",op,mem,s);
}
void checkCache(int tag, int set, int assoc, int verbose, int *lru, int *counter, struct trace t) {
    //this is the "main" function that does most of the heavy lifting
    //the basic structure is as follows:
    //1. checks the set for a match
    //2. if no match is found looks for an open space
    //3. if no open space is found first finds the lowest lru then evicts that line
    int evict = 0;
    //hit
    for(int i = 0; i < assoc; i++) { //checks set
        if(cache[set][i].valid == 1 && cache[set][i].tag == tag) {
            //found a match
            cache[set][i].lru = lru[0];
            counter[0]++; //hit counter
            if(verbose == 1){
                printTrace(t);
                printf(" hit\n");
            }
            //printf("found match");
            return;
        }
    }
    //miss
    counter[1]++; //miss counter
    if(verbose == 1) {
        printTrace(t);
        printf(" miss\n");
    }
    //printf("no match");
    
    for(int i = 0; i < assoc; i++) { //checks for an empty spot
        if(cache[set][i].valid == 0){
            //found an empty spot
            cache[set][i].tag = tag;
            cache[set][i].valid = 1;
            cache[set][i].lru = lru[0];
            return;
        }
    }
    //eviction
    for(int i = 0; i < assoc; i++) { //finds the lowest lru number
        if(cache[set][i].lru < cache[set][evict].lru) {
            evict = i;
        }
    }
    //printf("evicting %d\n",evict);
    //overwrites the line with the lowest lru
    cache[set][evict].tag = tag;
    cache[set][evict].lru = lru[0];
    counter[2]++; //eviction counter
    if(verbose == 1) {
        printTrace(t);
        printf(" eviction\n");
    }
    return;
};
// ============================================================= end of helpers =============================================================
int main(int argc, char *argv[])
{
    int *counter = (int*)malloc(sizeof(int)*3); //counter for hits misses and evictions
                                                //index 0 is hits, 1 is misses, 2 is evictions
    //printf("working\n");
    //unsigned char *c;
    int *lru = (int*)malloc(sizeof(int)); //lru counter
    int indexBits; //s
    int assoc; //E
    int blockBits; //b
    int verbose = 0; //v
    char *traceFile = "default"; //t
    int opt;
    //parses input args
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) { //looked up how to use getopt online
        switch (opt) {
            case 'h':
            case 'v':
                verbose = 1;
                printf("v: %d\n",verbose);
                break;
            case 's':
                indexBits = atoi(optarg); //did research on how to make the strings into ints and found atoi
                printf("s: %d\n",indexBits);
                break;
            case 'E':
                assoc = atoi(optarg);
                printf("E: %d\n",assoc);
                break;
            case 'b':
                blockBits = atoi(optarg);
                printf("b: %d\n",blockBits);
                break;
            case 't':
                traceFile = optarg;
                break;
        }
    }
    
    //initialize sets
    int numSets = pow(2,indexBits);
    FILE *f = fopen(traceFile , "r");
    unsigned char op = 0;
    unsigned long address = 0;
    unsigned int size = 0;
    int arrLen = 0;
    
    while(fscanf(f, " %c %lx,%d ", &op, &address, &size) > 0) {
        //gets array size
        printf("%c ",op);
        printf("%lx ",address);
        printf("%d \n",size);
        arrLen++;
    }
    struct trace *traceArr = (struct trace*)malloc(sizeof(struct trace) * arrLen);
    rewind(f);
    int i = 0;
    while(fscanf(f, " %c %lx,%d ", &op, &address, &size) > 0) {
        //make array of traces
        struct trace t;
        t.operation = op;
        t.memory = address;
        t.tsize = size;
        traceArr[i] = t;
        i++;
    }
    //initializing cache
    cache = (cacheLine**)malloc(numSets * sizeof(cacheLine*));
    for (int i = 0; i < numSets; i++) {
        cache[i] = (cacheLine*)malloc(assoc * sizeof(cacheLine));
    }
    struct trace curr;
    unsigned int set;
    unsigned int tag;
    address = 0;
    char operation;
    lru[0] = 0;
    
    //cache simulation
    for (int i = 0; i < arrLen; i++) {
        lru[0]++;
        //printf("iterate\n");
        //gets the trace
        curr = traceArr[i];
        operation = curr.operation;
        address = curr.memory;
        //size = curr.tsize;
        
        //printf("%ld\n",address);
        
        //parses trace
        set = getSet(blockBits, indexBits, address);
        tag = getTag(blockBits, indexBits, address);
        
        //printf("o: %d, s: %d, t: %ld\n",offset,set,tag);
        
        //checks the cache
        if (operation == 'L' || operation == 'S') {
            checkCache(tag, set,  assoc, verbose, lru, counter, curr);
        } else if (operation == 'M') {
            checkCache(tag, set,  assoc, verbose, lru, counter, curr);
            checkCache(tag, set, assoc, verbose, lru, counter, curr);
        }
        
    }
    
    printSummary(counter[0], counter[1], counter[2]);
}
