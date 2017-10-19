#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>

#define ulong unsigned long
#define bool char
#define true (char)1
#define false (char)0
#define DEBUG 1
#define DEB if(DEBUG)
#define nullptr 0

#if DEBUG
   #define new new1
   #define delete delete1
#endif

void initMemory(); // Initialize memory structure
void freeAll();    // Clears all the memory objects
#define MEMORY_ERROR 1

// BEG Memory structures //
typedef struct _MemObj {
    ulong size; // size of malloced memory
    void *ptr; // pointer to the malloced memory
    struct _MemObj *nextObj;
} MemObj;

typedef struct _Memory {
    MemObj *root;
    MemObj *last;
    long mallocCalls; // counts malloc calls
    long freeCalls;   // counts free calls
} Memory;
// END Memory structures //


Memory *memory;
/**
 * Init memory structure, so new method could be used
 */
void initMemory() {
    memory = (Memory *) malloc(sizeof(Memory));
    if (nullptr == memory) {
        DEB printf("Memory error!\nCould not initialize memory!\n");
        exit(MEMORY_ERROR);
    }
    memory->root = nullptr;
    memory->last = nullptr;
    memory->mallocCalls = 1;
    memory->freeCalls = 0;
}

/**
 * Frees all memory, malloced with new calls and destroys memory structure itself
 */
void freeAll() {
    MemObj *ptr = memory->root;
    MemObj *nxt;
    while (nullptr != ptr) {
        nxt = ptr->nextObj;
        ptr->nextObj = nullptr; // for sure
        ptr->size = 0;
        free(ptr->ptr);
        free(ptr);
        memory->freeCalls += 2;
        ptr = nxt;
    }
    DEB printf("Memory info:\nmalloc calls:\t%ld\nfree calls:\t%ld\n", memory->mallocCalls,
               memory->freeCalls + 1); // +1 for next line
    free(memory);
}
/**
 * Method "new" mallocs the memory, creating the MemObj. All the MemObj-s are removed automatically through freeAll call
 * @param size number of bytes
 * @return pointer to the given memory
 */
void *new(size_t size) {
    MemObj *obj = (MemObj *) malloc(sizeof(MemObj)); // Try to malloc the memory
    if (nullptr == obj) { // in case of error
        DEB printf("Memory error!\nCould not initialize memory!\n");
        freeAll();
        exit(MEMORY_ERROR);
    } else { // operation OK
        memory->mallocCalls++;
    }
    obj->nextObj = nullptr;

    obj->ptr = malloc(size); // Try to malloc the memory
    if (nullptr == obj->ptr) {
        DEB printf("Memory error!\nCould not malloc %zu bytes of memory!\n", size);
        free(obj);
        memory->freeCalls++;
        freeAll();
        exit(MEMORY_ERROR);
    } else { // succeeded
        memory->mallocCalls++;
    }
    obj->size = size;

    if (nullptr == memory->root) { // Setting up the memory root
        memory->root = obj;
        memory->last = obj;
    } else { // adding memory object
        memory->last->nextObj = obj;
        memory->last = obj;
    }
    return obj->ptr;
}

// BEG Structures //
typedef struct _ULongArray {
    ulong length;
    ulong *element;
} ULongArray;

typedef struct _BoolArray {
    ulong length;
    bool *element;
} BoolArray;
// END Structures //

// BEG Constructor equivalents //
ULongArray *newULongArray(ulong n) {
    ULongArray *obj = (ULongArray *) new(sizeof(ULongArray));
    obj->element = (ulong *) new(sizeof(ulong) * n);
    obj->length = n;
    return obj;
}

BoolArray *newBoolArray(ulong n) {
    BoolArray *obj = (BoolArray *) new(sizeof(BoolArray));
    obj->element = (bool *) new(sizeof(bool) * n);
    obj->length = n;
    return obj;
}
// END Constructor equivalents //

ulong getPrimeForIndex(ulong index) {
    return 2 * index + 3;
}
/**
 * Method generates the Eratosthenes sieve
 * @param upTo bound
 * @return the array of primes up to the bound
 */
ULongArray *genSieveOfEratosthenes(ulong upTo) {
    upTo = (upTo - 1) / 2; // memory improvements
    if (0 == upTo) { // trivial
        ULongArray *primes = newULongArray(1);
        primes->element[0] = 0;
        return primes;
    }
    BoolArray *isPrime = newBoolArray(upTo);
    for (ulong k = 0; k < isPrime->length; k++) { // Init array with true
        isPrime->element[k] = true;
    }
    ulong beg = 0;
    ulong step = getPrimeForIndex(0);
    while (beg < upTo) { // Cross out numbers, that are not primes
        for (ulong i = beg + step; i < upTo; i += step) {
            isPrime->element[i] = false;
        }
        beg++;
        while (beg < upTo && !isPrime->element[beg])
            beg++;
        step = getPrimeForIndex(beg);
    }
    ULongArray *primes = newULongArray(upTo);
    primes->element[0] = 2; // starting with 2
    ulong writeIndex = 1;
    for (ulong i = 0; i < isPrime->length; i++) {
        if (isPrime->element[i]) {
            primes->element[writeIndex] = getPrimeForIndex(i); // Write primes to array
            writeIndex++;
        }
    }
    primes->length = writeIndex;
    return primes;
}
/**
 * Method for factorization the number
 * @param numb number to factorize
 * @return the array of primes, that are dividers of numb
 */
ULongArray *factorize(ulong numb) {
    ULongArray *allPrimes = genSieveOfEratosthenes((ulong) sqrt(numb) + 1); // get array of primes
    const int MAX_PRIMES = 65; // maximum number of primes
    ULongArray *primesOfNumb = newULongArray(MAX_PRIMES);
    primesOfNumb->element[0] = (ulong) 1;
    ulong writeIndex = 1;
    ulong i = 0;
    while (0 == numb % 2) {
        primesOfNumb->element[writeIndex++] = 2;
        numb /= 2;
    }
    while (numb > 1 && i < allPrimes->length) {
        while (numb > 1 && 0 == numb % allPrimes->element[i]) {
            numb /= allPrimes->element[i];
            primesOfNumb->element[writeIndex++] = allPrimes->element[i];
        }
        i++;
    }
    if (numb != 1)
        primesOfNumb->element[writeIndex++] = numb;
    primesOfNumb->length = writeIndex;
    return primesOfNumb;
}

//#pragma warning(disable : 4996)
int main() {
    initMemory(); // Initialize auto memory
    long x;
    char *inp = (char *) new(sizeof(char) * 255); // input number
    char *temp = (char *) new(sizeof(char) * 255);
    scanf("%s", inp);
    x = atol(inp);
    sprintf(temp, "%ld", x);
    if (x > 0 && !strcmp(temp, inp)) {
        ULongArray *primes = factorize((ulong) x);
        for (ulong i = 0; i < primes->length; i++) {
            printf("%lu ", primes->element[i]);
        }
    } else { // in case of error
        printf("[error]\n");
    }

    freeAll(); // Free memory automatically
    return 0;
}
