// HashTable.c ... implementation of HashTable ADT
// Written by John Shepherd, May 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "HashTable.h"
#include "List.h"

// Types and functions local to HashTable ADT

typedef struct HashTabRep {
	List *lists;  // either use this
	int   nslots; // # elements in array
	int   nitems; // # items stored in HashTable
} HashTabRep;

// convert key into index (from Sedgewick)
unsigned int hash(Key k, int tableSize)
{
	unsigned int h = 0;
	int a = 31415, b = 27183;
	for (; *k != '\0'; k++) {
		a = a*b % (tableSize-1);
		h = (a*h + *k) % tableSize;
	}
	return (h % tableSize);
}


// Interface functions for HashTable ADT

// create an empty HashTable
HashTable newHashTable(int N)
{
	HashTabRep *new = malloc(sizeof(HashTabRep));
	assert(new != NULL);
	new->lists = malloc(N*sizeof(List));
	assert(new->lists != NULL);
	int i;
	for (i = 0; i < N; i++)
		new->lists[i] = newList();
	new->nslots = N; new->nitems = 0;
	return new;
}

// free memory associated with HashTable
void dropHashTable(HashTable ht)
{
	assert(ht != NULL);
	int i;
    for (i = 0; i < ht->nslots; i++)
		dropList(ht->lists[i]);
	free(ht->lists);
	free(ht);
}

// display HashTable stats
void HashTableStats(HashTable ht)
{
	assert(ht != NULL);
	printf("Hash Table Stats:\n");
	printf("Number of slots = %d\n", ht->nslots);
	printf("Number of items = %d\n", ht->nitems);
	printf("Chain length distribution\n");
	printf("%8s %8s\n", "Length", "#Chains");

	size_t lengthChainsLength = 4;
	size_t *lengthChains = calloc(lengthChainsLength, sizeof(size_t));
	for (size_t l = 0; l < (size_t)ht->nslots; ++l) {
		size_t chainLength = ListLength(ht->lists[l]);
		if (chainLength >= lengthChainsLength) {
			lengthChains = realloc(lengthChains, lengthChainsLength * 2 * sizeof(size_t));
			memset(lengthChains + lengthChainsLength, 0, lengthChainsLength * sizeof(size_t));
			lengthChainsLength *= 2;
		}

		++lengthChains[chainLength];
	}

	for (size_t l = 0; l < lengthChainsLength; ++l)
		if (lengthChains[l] != 0)
			printf("%8zu %8zu\n", l, lengthChains[l]);

	free(lengthChains);
}

// insert a new value into a HashTable
void HashTableInsert(HashTable ht, Item it)
{
	assert(ht != NULL);
	int i = hash(key(it), ht->nslots);
	if (ListSearch(ht->lists[i], key(it)) == NULL) {
		ListInsert(ht->lists[i], it);
		ht->nitems++;
	}
}

// delete a value from a HashTable
void HashTableDelete(HashTable ht, Key k)
{
	assert(ht != NULL);
	int h = hash(k, ht->nslots);
	ListDelete(ht->lists[h], k);
}

// get Item from HashTable using Key
Item *HashTableSearch(HashTable ht, Key k)
{
	assert(ht != NULL);
	int i = hash(k, ht->nslots);
	return ListSearch(ht->lists[i], k);
}
