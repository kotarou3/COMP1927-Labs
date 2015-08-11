// testList.c - testing DLList data type
// Written by John Shepherd, March 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "DLList.h"

int main(int argc, char *argv[])
{
    DLList myList;
    myList = getDLList(stdin);
    showDLList(stdout, myList);
    assert(validDLList(myList));
    return 0;
}
