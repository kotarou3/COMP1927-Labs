// crawl.c ... build a graph of part of the web
// Written by John Shepherd, September 2015
// Uses the cURL library and functions by Vincent Sanders <vince@kyllikki.org>

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <curl/curl.h>
#include "graph.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "url_file.h"

#define BUFSIZE (1 << 15)

void setFirstURL(char *, char *);
void resolveURL(const char *, const char *, char *, size_t);
const char *nextURL(const char *);
bool getURL(const char *, char *, size_t);

int main(int argc, char **argv)
{
	char firstURL[BUFSIZE];
	int  maxURLs;

	if (argc > 2) {
		setFirstURL(argv[1], firstURL);
		maxURLs = atoi(argv[2]);
		if (maxURLs < 40) maxURLs = 40;
	}
	else {
		fprintf(stderr, "Usage: %s BaseURL MaxURLs\n", argv[0]);
		exit(1);
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);

	Queue queue = newQueue();
	Graph graph = newGraph(maxURLs);
	Set seen = newSet();

	enterQueue(queue, firstURL);
	for (; !emptyQueue(queue) && maxURLs > 0; --maxURLs) {
		char* currentURL = leaveQueue(queue);

		if (strncmp(currentURL, "http://www.cse.unsw.edu.au/", strlen("http://www.cse.unsw.edu.au/"))) {
			fprintf(stderr, "Skipping non-CSE address: %s\n", currentURL);
			free(currentURL);
			continue;
		}
		fprintf(stderr, "Scraping %s\n", currentURL);

		URL_FILE *handle = url_fopen(currentURL, "r");
		if (!handle) {
			fprintf(stderr, "Warning: Couldn't open %s\n", currentURL);
			free(currentURL);
			continue;
		}

		while (!url_feof(handle)) {
			char buffer[BUFSIZE];
			const char* cur = url_fgets(buffer, sizeof(buffer), handle);

			while ((cur = nextURL(cur))) {
				char link[BUFSIZE], normalisedLink[BUFSIZE];

				getURL(cur, link, BUFSIZE - 1);
				resolveURL(currentURL, link, normalisedLink, BUFSIZE);

				addEdge(graph, currentURL, normalisedLink);
				if (!isElem(seen, normalisedLink)) {
					enterQueue(queue, normalisedLink);
					insertInto(seen, normalisedLink);
				}

				cur += strlen(link);
			}
		}

		url_fclose(handle);
		free(currentURL);

		struct timespec delay = {
			.tv_sec = 0,
			.tv_nsec = 100 * 1000 * 1000
		};
		nanosleep(&delay, 0);
	}

	showGraph(graph, 2);

	disposeSet(seen);
	disposeGraph(graph);
	disposeQueue(queue);

	curl_global_cleanup();

	return 0;
}

// resolveURL(From, To, Out, N)
// - Take a base URL, and a href URL, and resolve them as a browser would for an anchor tag
// - Also strips out any anchors
// XXX: Assumes valid HTTP URLs with protocol and path - probably will segfault if not
void resolveURL(const char *from, const char *to, char *out, size_t n)
{
	if (strstr(to, "://")) {
		// Absolute
		strncpy(out, to, n);
	}
	else {
		size_t endIndex;
		if (!strncmp(to, "//", 2))
			// Protocol relative
			endIndex = strchr(from, ':') - from + 1;
		else if (to[0] == '/')
			// Domain relative
			endIndex = strchr(strstr(from, "://") + 4, '/') - from;
		else
			// Directory relative
			endIndex = strrchr(from, '/') - from + 1;

		strncpy(out, from, endIndex);
		strncpy(out + endIndex, to, n - endIndex);
	}

	// Drop the anchor
	*strchrnul(out, '#') = 0;

	// Naive removal of ../ and ./
	Stack pathComponents = newStack();
	char *pathStart = strstr(out, "//") + 2;
	for (char *str = pathStart, *saveptr, *token; (token = strtok_r(str, "/", &saveptr)); str = 0)
		if (!strcmp(token, "."))
			continue;
		else if (!strcmp(token, ".."))
			free(popFrom(pathComponents));
		else
			pushOnto(pathComponents, token);

	// No dequeue, so just push it in to another stack to reverse the order
	Stack pathComponents2 = newStack();
	while (!emptyStack(pathComponents)) {
		char *component = popFrom(pathComponents);
		pushOnto(pathComponents2, component);
		free(component);
	}

	pathStart[-1] = 0; // Terminate `out` at the end of the protocol
	while (!emptyStack(pathComponents2)) {
		char *component = popFrom(pathComponents2);
		strcat(out, "/");
		strcat(out, component);
		free(component);
	}

	disposeStack(pathComponents2);
	disposeStack(pathComponents);
}

// setFirstURL(Base, First)
// - sets a "normalised" version of Base as First
// - modifies Base to a "normalised" version of itself
void setFirstURL(char *base, char *first)
{
	char *c;
	if ((c = strstr(base, "/index.html")) != NULL) {
		strcpy(first, base);
		*c = '\0';
	}
	else if (base[strlen(base) - 1] == '/') {
		strcpy(first, base);
		strcat(first, "index.html");
		base[strlen(base)-1] = '\0';
	}
	else {
		strcpy(first, base);
		strcat(first, "/index.html");
	}
}

// nextURL(S)
// - finds start of next URL in string S
// - returns NULL if no (more) URLs
// - finds URLs by searching for "href" case-insensitively
const char *nextURL(const char *s)
{
	if (!s) return NULL;

	const char *c;
	// find next "href" in string
	c = strcasestr(s, "href");
	if (c == NULL) return NULL;
	// skip over "href" to start of URL
	while (*c != '\0') {
		if (*c == '"' || *c == '\'') break;
		c++;
	}
	if (*c == '\0') return NULL;
	c++; // set to start of URL
	return c;
}

// getURL(S, Buf, N)
// - copies URL starting at S into Buf
// - stops copying when either
//   - reaches a ' or "
//   - reaches end of S
//   - has copied N-1 chars
// - return value indicates whether URL is complete
//   - returns 1 if stopped by ' or "
//   - returns 0 otherwise
// - appends '\0' to Buf (included in N)
bool getURL(const char *src, char *dest, size_t N)
{
	const char *c = src;
	char *d = dest;
	size_t nc = 0;
	while (nc < N && *c != '\0') {
		if (*c == '"' || *c == '\'') break;
		*d++ = *c++;
		nc++;
	}
	*d = '\0';
	return (nc < N && *c != '\0');
}
