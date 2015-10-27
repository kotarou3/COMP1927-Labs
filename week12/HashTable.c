// HashTable.c ... implementation of HashTable ADT
// Written by John Shepherd, May 2013

#include <nmmintrin.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "HashTable.h"

// Types and functions local to HashTable ADT

typedef struct HashTabSlot {
	uint32_t crc; // for faster rehashing
	Item item;
} HashTabSlot;

typedef struct HashTabRep {
	HashTabSlot *slots;  // either use this
	int   nslots; // # elements in array
	int   nitems; // # items stored in HashTable
} HashTabRep;

static const Item HashTableDeletedSentinel = "0cc96b9f-ca19-4a9f-a568-be880e6d2414";

static unsigned int crc32c(Key k)
{
	const size_t length = strlen(k);
	uint32_t crc = 0;
	size_t c = 0;

	// Lots of potentially unaligned reads, but good enough for short key sizes
	for (; length - c >= sizeof(uint64_t); c += sizeof(uint64_t))
		crc = _mm_crc32_u64(crc, *(uint64_t*)(k + c));
	for (; c < length; ++c)
		crc = _mm_crc32_u8(crc, k[c]);

	return crc;
}


// Interface functions for HashTable ADT

// create an empty HashTable
HashTable newHashTable(int N)
{
	HashTabRep *new = malloc(sizeof(HashTabRep));
	assert(new != NULL);
	new->slots = calloc(N, sizeof(HashTabSlot));
	assert(new->slots != NULL);
	new->nslots = N; new->nitems = 0;
	return new;
}

// free memory associated with HashTable
void dropHashTable(HashTable ht)
{
	assert(ht != NULL);
	for (size_t i = 0; i < (size_t)ht->nslots; i++)
		if (ht->slots[i].item && ht->slots[i].item != HashTableDeletedSentinel)
			free(ht->slots[i].item);
	free(ht->slots);
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
		size_t chainLength = 0;
		if (ht->slots[l].item && ht->slots[l].item != HashTableDeletedSentinel)
			chainLength = (l - ht->slots[l].crc % ht->nslots) % ht->nslots + 1;

		if (chainLength >= lengthChainsLength) {
			size_t multiplier = 2;
			while (lengthChainsLength * multiplier < chainLength)
				multiplier += multiplier;

			lengthChains = realloc(lengthChains, lengthChainsLength * multiplier * sizeof(size_t));
			memset(lengthChains + lengthChainsLength, 0, lengthChainsLength * (multiplier - 1) * sizeof(size_t));
			lengthChainsLength *= multiplier;
		}

		++lengthChains[chainLength];
	}

	for (size_t l = 0; l < lengthChainsLength; ++l)
		if (lengthChains[l] != 0)
			printf("%8zu %8zu\n", l, lengthChains[l]);

	free(lengthChains);
}

static void HashTableInsertSlot(HashTable ht, HashTabSlot slot)
{
	// Could insert duplicates, but won't affect integrity
	size_t key = slot.crc % ht->nslots;
	while (ht->slots[key].item && ht->slots[key].item != HashTableDeletedSentinel) {
		if (!strcmp(ht->slots[key].item, slot.item)) {
			free(slot.item);
			return;
		} else {
			key = (key + 1) % ht->nslots;
		}
	}

	ht->slots[key] = slot;
	++ht->nitems;
}

static void HashTableRehash(HashTable ht, size_t newTableSize)
{
	assert(ht != NULL);
	assert(newTableSize >= (size_t)ht->nitems);

	size_t oldSlotsLength = ht->nslots;
	HashTabSlot *oldSlots = ht->slots;

	ht->nitems = 0;
	ht->nslots = newTableSize;
	ht->slots = calloc(newTableSize, sizeof(HashTabSlot));
	assert(ht->slots != NULL);

	for (size_t s = 0; s < oldSlotsLength; ++s)
		if (oldSlots[s].item && oldSlots[s].item != HashTableDeletedSentinel)
			HashTableInsertSlot(ht, oldSlots[s]);

	free(oldSlots);
}

// insert a new value into a HashTable
void HashTableInsert(HashTable ht, Item it)
{
	assert(ht != NULL);

	if (ht->nitems >= ht->nslots * 2 / 3)
		HashTableRehash(ht, ht->nslots * 2);

	HashTableInsertSlot(ht, (HashTabSlot){.crc = crc32c(it), .item = strdup(it)});
}

static HashTabSlot *HashTableSlotSearch(HashTable ht, Key k)
{
	assert(ht != NULL);
	for (size_t key = crc32c(k) % ht->nslots; ht->slots[key].item; key = (key + 1) % ht->nslots)
		if (ht->slots[key].item != HashTableDeletedSentinel && !strcmp(ht->slots[key].item, k))
			return &ht->slots[key];
	return NULL;
}

// delete a value from a HashTable
void HashTableDelete(HashTable ht, Key k)
{
	assert(ht != NULL);

	HashTabSlot *slot = HashTableSlotSearch(ht, k);
	if (slot) {
		free(slot->item);
		slot->item = HashTableDeletedSentinel;
	}
}

// get Item from HashTable using Key
Item *HashTableSearch(HashTable ht, Key k)
{
	assert(ht != NULL);
	HashTabSlot *slot = HashTableSlotSearch(ht, k);
	if (slot)
		return &slot->item;
	return NULL;
}
