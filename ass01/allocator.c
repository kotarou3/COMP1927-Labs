//
//  COMP1927 Assignment 1 - Vlad: the memory allocator
//  allocator.c ... implementation
//
//  Created by Liam O'Connor on 18/07/12.
//  Modified by John Shepherd in August 2014, August 2015
//  Copyright (c) 2012-2015 UNSW. All rights reserved.
//

#include "allocator.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef _ISOC11_SOURCE
    // For memalign()
    #include <malloc.h>
#endif

#define HEADER_SIZE    sizeof(struct free_list_header)
#define MIN_ALLOC (1 << 5) // Double the header size
#define MIN_INIT_ALLOC (1 << 9)
#define MAGIC_FREE     0xDEADBEEF
#define MAGIC_ALLOC    0xBEEFDEAD

typedef unsigned char byte;
typedef u_int32_t vlink_t;
typedef u_int32_t vsize_t;
typedef u_int32_t vaddr_t;

typedef struct free_list_header {
    u_int32_t magic; // ought to contain MAGIC_FREE
    vsize_t size;    // # bytes in this block (including header)
    vlink_t next;    // memory[] index of next free block
    vlink_t prev;    // memory[] index of previous free block
} free_header_t;

// Global data

static byte *memory = NULL;   // pointer to start of allocator memory
static vaddr_t free_list_ptr; // index in memory[] of first block in free list
static vsize_t memory_size;   // number of bytes malloc'd in memory[]

static inline u_int32_t get_block_size(u_int32_t n)
{
    if (n <= MIN_ALLOC)
        return MIN_ALLOC;

    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    ++n;

    return n;
}

static inline free_header_t *indexToNode(vlink_t index)
{
    return (free_header_t *)&memory[index];
}

static inline vlink_t nodeToIndex(free_header_t *node)
{
    return (byte *)node - memory;
}

// Input: size - number of bytes to make available to the allocator
// Output: none
// Precondition: Size is a power of two.
// Postcondition: `size` bytes are now available to the allocator
//
// (If the allocator is already initialised, this function does nothing,
//  even if it was initialised with different size)

void vlad_init(u_int32_t size)
{
    size = size < MIN_INIT_ALLOC ? MIN_INIT_ALLOC : get_block_size(size);
#ifdef _ISOC11_SOURCE
    memory = aligned_alloc(size, size);
#else
    memory = memalign(size, size);
#endif
    if (!memory) {
        fprintf(stderr, "vlad_init: insufficient memory");
        abort();
    }

    free_header_t *node = indexToNode(0);
    node->magic = MAGIC_FREE;
    node->size = size;
    node->next = 0;
    node->prev = 0;

    free_list_ptr = 0;
    memory_size = size;
}


// Input: n - number of bytes requested
// Output: p - a pointer, or NULL
// Precondition: n is < size of memory available to the allocator
// Postcondition: If a region of size n or greater cannot be found, p = NULL
//                Else, p points to a location immediately after a header block
//                      for a newly-allocated region of some size >=
//                      n + header size.

void *vlad_malloc(u_int32_t size)
{
    if (!memory)
        return NULL;

    size = get_block_size(HEADER_SIZE + size);

    free_header_t *node = indexToNode(free_list_ptr);
    free_header_t *bestFitNode = NULL;
    while (true) {
        if (node->magic != MAGIC_FREE) {
            fprintf(stderr, "Memory corruption");
            abort();
        }

        if (size <= node->size && (!bestFitNode || node->size < bestFitNode->size))
            bestFitNode = node;

        if (node->next == free_list_ptr)
            break;

        node = indexToNode(node->next);
    }
    node = bestFitNode;

    if (!node || node->size < size)
        return NULL;

    while (node->size > size) {
        vsize_t newSize = node->size / 2;

        free_header_t *newNode = (free_header_t *)((byte *)node + newSize);
        newNode->magic = MAGIC_FREE;
        newNode->size = newSize;
        newNode->next = node->next;
        newNode->prev = nodeToIndex(node);
        indexToNode(newNode->next)->prev = nodeToIndex(newNode);

        node->size = newSize;
        node->next = nodeToIndex(newNode);
    }

    if (indexToNode(node->next) == node)
        return NULL;

    indexToNode(node->next)->prev = node->prev;
    indexToNode(node->prev)->next = node->next;
    if (free_list_ptr == nodeToIndex(node))
        free_list_ptr = node->next;

    node->magic = MAGIC_ALLOC;

    return (byte *)node + HEADER_SIZE;
}


// Input: object, a pointer.
// Output: none
// Precondition: object points to a location immediately after a header block
//               within the allocator's memory.
// Postcondition: The region pointed to by object can be re-allocated by
//                vlad_malloc

void vlad_free(void *object)
{
    free_header_t *node = (free_header_t *)((byte *)object - HEADER_SIZE);
    if (node->magic != MAGIC_ALLOC) {
        fprintf(stderr, "Attempt to free non-allocated memory");
        abort();
    }
    if (node->size != get_block_size(node->size)) {
        fprintf(stderr, "Memory corruption");
        abort();
    }

    node->magic = MAGIC_FREE;

    bool gotNextFreeNode = false;
    while (true) {
        free_header_t *buddyNode = (free_header_t *)((size_t)node ^ node->size);
        if (buddyNode->magic == MAGIC_ALLOC || buddyNode->size != node->size) {
            break;
        } else if (buddyNode->magic != MAGIC_FREE) {
            fprintf(stderr, "Memory corruption");
            abort();
        }

        if (!gotNextFreeNode) {
            if (node < buddyNode) {
                if (indexToNode(buddyNode->next) == buddyNode) {
                    assert(buddyNode->next == free_list_ptr);

                    node->next = nodeToIndex(node);
                    node->prev = nodeToIndex(node);
                    free_list_ptr = nodeToIndex(node);
                } else {
                    node->next = buddyNode->next;
                    node->prev = buddyNode->prev;
                    indexToNode(node->next)->prev = nodeToIndex(node);
                    indexToNode(node->prev)->next = nodeToIndex(node);
                    if (free_list_ptr == nodeToIndex(buddyNode))
                        free_list_ptr = nodeToIndex(node);
                }
            } else {
                node = buddyNode;
            }
        } else {
            if (node < buddyNode) {
                assert(indexToNode(node->next) == buddyNode);

                indexToNode(buddyNode->next)->prev = nodeToIndex(node);
                node->next = buddyNode->next;
            } else {
                assert(indexToNode(node->prev) == buddyNode);

                indexToNode(node->next)->prev = nodeToIndex(buddyNode);
                buddyNode->next = node->next;
                node = buddyNode;
            }
        }
        node->size *= 2;

        gotNextFreeNode = true;
    }

    if (!gotNextFreeNode) {
        free_header_t *prevNode = indexToNode(indexToNode(free_list_ptr)->prev);
        while (true) {
            if (prevNode->magic != MAGIC_FREE) {
                fprintf(stderr, "Memory corruption");
                abort();
            }

            if (prevNode < node)
                break;

            if (nodeToIndex(prevNode) == free_list_ptr) {
                // Reached the start of the list without finding a lower addressed node, so prepend to it
                prevNode = indexToNode(prevNode->prev);
                free_list_ptr = nodeToIndex(node);
                break;
            }

            prevNode = indexToNode(prevNode->prev);
        }

        node->prev = nodeToIndex(prevNode);
        node->next = prevNode->next;
        indexToNode(prevNode->next)->prev = nodeToIndex(node);
        prevNode->next = nodeToIndex(node);
    }
}


// Stop the allocator, so that it can be init'ed again:
// Precondition: allocator memory was once allocated by vlad_init()
// Postcondition: allocator is unusable until vlad_int() executed again

void vlad_end(void)
{
    free(memory);
    memory = NULL;
}


// Precondition: allocator has been vlad_init()'d
// Postcondition: allocator stats displayed on stdout

void vlad_stats(void)
{
    free_header_t *node = indexToNode(free_list_ptr);
    size_t n = 0;
    while (true) {
        printf("%zu:\t%p:%u\n", ++n, (void*)node, node->size);

        assert(node->magic == MAGIC_FREE);
        assert(indexToNode(node->next)->prev == nodeToIndex(node));
        assert(((size_t)node & ~(node->size - 1)) == (size_t)node);

        free_header_t *buddyNode = (free_header_t *)((size_t)node ^ node->size);
        assert(buddyNode->magic != MAGIC_FREE || buddyNode->size != node->size);

        if (node->next == free_list_ptr) {
            break;
        } else {
            assert(indexToNode(node->next) > node);
            assert((byte *)node + node->size <= (byte *)indexToNode(node->next));
        }

        node = indexToNode(node->next);
    }
}


//
// All of the code below here was written by Alen Bou-Haidar, COMP1927 14s2
//

//
// Fancy allocator stats
// 2D diagram for your allocator.c ... implementation
//
// Copyright (C) 2014 Alen Bou-Haidar <alencool@gmail.com>
//
// FancyStat is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// FancyStat is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>


#include <string.h>

#define STAT_WIDTH  32
#define STAT_HEIGHT 16
#define BG_FREE      "\x1b[48;5;35m"
#define BG_ALLOC     "\x1b[48;5;39m"
#define FG_FREE      "\x1b[38;5;35m"
#define FG_ALLOC     "\x1b[38;5;39m"
#define CL_RESET     "\x1b[0m"


typedef struct point {int x, y;} point;

static point offset_to_point(int offset,  int size, int is_end);
static void fill_block(char graph[STAT_HEIGHT][STAT_WIDTH][20],
                        int offset, char * label);



// Print fancy 2D view of memory
// Note, This is limited to memory_sizes of under 16MB
void vlad_reveal(void *alpha[26])
{
    int i, j;
    vlink_t offset;
    char graph[STAT_HEIGHT][STAT_WIDTH][20];
    char free_sizes[26][32];
    char alloc_sizes[26][32];
    char label[3]; // letters for used memory, numbers for free memory
    int free_count, alloc_count, max_count;
    free_header_t * block;

    // initilise size lists
    for (i=0; i<26; i++) {
        free_sizes[i][0]= '\0';
        alloc_sizes[i][0]= '\0';
    }

    // Fill graph with free memory
    offset = 0;
    i = 1;
    free_count = 0;
    while (offset < memory_size){
        block = (free_header_t *)(memory + offset);
        if (block->magic == MAGIC_FREE) {
            snprintf(free_sizes[free_count++], 32,
                "%d) %d bytes", i, block->size);
            snprintf(label, 3, "%d", i++);
            fill_block(graph, offset,label);
        }
        offset += block->size;
    }

    // Fill graph with allocated memory
    alloc_count = 0;
    for (i=0; i<26; i++) {
        if (alpha[i] != NULL) {
            offset = ((byte *) alpha[i] - (byte *) memory) - HEADER_SIZE;
            block = (free_header_t *)(memory + offset);
            snprintf(alloc_sizes[alloc_count++], 32,
                "%c) %d bytes", 'a' + i, block->size);
            snprintf(label, 3, "%c", 'a' + i);
            fill_block(graph, offset,label);
        }
    }

    // Print all the memory!
    for (i=0; i<STAT_HEIGHT; i++) {
        for (j=0; j<STAT_WIDTH; j++) {
            printf("%s", graph[i][j]);
        }
        printf("\n");
    }

    //Print table of sizes
    max_count = (free_count > alloc_count)? free_count: alloc_count;
    printf(FG_FREE"%-32s"CL_RESET, "Free");
    if (alloc_count > 0){
        printf(FG_ALLOC"%s\n"CL_RESET, "Allocated");
    } else {
        printf("\n");
    }
    for (i=0; i<max_count;i++) {
        printf("%-32s%s\n", free_sizes[i], alloc_sizes[i]);
    }

}

// Fill block area
static void fill_block(char graph[STAT_HEIGHT][STAT_WIDTH][20],
                        int offset, char * label)
{
    point start, end;
    free_header_t * block;
    char * color;
    char text[3];
    block = (free_header_t *)(memory + offset);
    start = offset_to_point(offset, memory_size, 0);
    end = offset_to_point(offset + block->size, memory_size, 1);
    color = (block->magic == MAGIC_FREE) ? BG_FREE: BG_ALLOC;

    int x, y;
    for (y=start.y; y < end.y; y++) {
        for (x=start.x; x < end.x; x++) {
            if (x == start.x && y == start.y) {
                // draw top left corner
                snprintf(text, 3, "|%s", label);
            } else if (x == start.x && y == end.y - 1) {
                // draw bottom left corner
                snprintf(text, 3, "|_");
            } else if (y == end.y - 1) {
                // draw bottom border
                snprintf(text, 3, "__");
            } else if (x == start.x) {
                // draw left border
                snprintf(text, 3, "| ");
            } else {
                snprintf(text, 3, "  ");
            }
            sprintf(graph[y][x], "%s%s"CL_RESET, color, text);
        }
    }
}

// Converts offset to coordinate
static point offset_to_point(int offset,  int size, int is_end)
{
    int pot[2] = {STAT_WIDTH, STAT_HEIGHT}; // potential XY
    int crd[2] = {0};                       // coordinates
    int sign = 1;                           // Adding/Subtracting
    int inY = 0;                            // which axis context
    int curr = size >> 1;                   // first bit to check
    point c;                                // final coordinate
    if (is_end) {
        offset = size - offset;
        crd[0] = STAT_WIDTH;
        crd[1] = STAT_HEIGHT;
        sign = -1;
    }
    while (curr) {
        pot[inY] >>= 1;
        if (curr & offset) {
            crd[inY] += pot[inY]*sign;
        }
        inY = !inY; // flip which axis to look at
        curr >>= 1; // shift to the right to advance
    }
    c.x = crd[0];
    c.y = crd[1];
    return c;
}
