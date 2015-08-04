// useIntList.c - testing IntList data type

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "IntList.h"

int main(int argc, char *argv[])
{
	IntList myList, myOtherList;

	myList = getIntList(stdin);
	assert(IntListOK(myList));

	myOtherList = IntListSortedCopy(myList);
	showIntList(myOtherList);
	assert(IntListOK(myOtherList));
	assert(IntListIsSorted(myOtherList));

	freeIntList(myList);
	freeIntList(myOtherList);

	return 0;
}
