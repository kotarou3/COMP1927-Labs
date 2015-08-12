// testList.c - testing DLList data type
// Written by John Shepherd, March 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "DLList.h"

void tInsertBefore(DLList L);
void tInsertAfter(DLList L);
void tDelete(DLList L);

int main(int argc, char *argv[])
{
    DLList myList;
    myList = getDLList(stdin);
    showDLList(stdout, myList);
    assert(validDLList(myList));
    tInsertBefore(myList);
    tInsertAfter(myList);
    tDelete(myList);
    return 0;
}

void tInsertBefore(DLList L)
{
    printf("===== Testing tInsertBefore =====\n");
    printf("Inserts integers 1-10 into the List before each other\n");
    printf("Current Element: %s\n", DLListCurrent(L));
    printf("Expected Output: 10 9 8 7 6 5 4 3 2 1 %s", DLListCurrent(L));
    showDLList(stdout, L);
    for (int i = 1; i <= 10; ++i) {
        char input[30];
        sprintf(input, "%d", i);
        DLListBefore(L, input);
    }
    showDLList(stdout, L);

    assert(validDLList(L));
    printf("===== End Test =====\n");
}

void tInsertAfter(DLList L)
{
    printf("===== Testing tInsertAfter =====\n");
    printf("Inserts integers 1-10 into the List after each other\n");
    printf("Current Element: %s\n", DLListCurrent(L));
    printf("Current List: ");
    showDLList(stdout, L);
    printf("Expected Output: %s 1 2 3 4 5 6 7 8 9 ", DLListCurrent(L));
    showDLList(stdout, L);
    for (int i = 1; i <= 10; ++i) {
        char input [30];
        sprintf(input, "%d", i);
        DLListAfter(L, input);
    }
    showDLList(stdout, L);

    assert(validDLList(L));
    printf("===== End Test =====\n");
}

void tDelete(DLList L){
    printf("===== Testing tDelete =====\n");
    printf("Current Element: %s\n", DLListCurrent(L));
    printf("Current List: ");
    showDLList(stdout, L);
    DLListMoveTo(L, 1);
    printf("Deleting First Element: %s\n", DLListCurrent(L));
    DLListDelete(L);
    printf("Current List: ");
    showDLList(stdout, L);
    printf("Deleting Last Element: %s\n", DLListCurrent(L));
    printf("List Length: %d\n", DLListLength(L));
    DLListMoveTo(L, DLListLength(L));
    DLListDelete(L);
    printf("Current List: ");
    showDLList(stdout, L);

    assert(validDLList(L));
    printf("===== End Test =====\n");
}
