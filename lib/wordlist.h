#ifndef __WORDLIST_HEADER__
#define __WORDLIST_HEADER__

#include "ds/darray.h"

#define WORDLIST_INITIAL_SIZE 1024
#define WORDLIST_WORD_SIZE 128


typedef struct {
    Darray *words;
} Wordlist;

Wordlist *Wordlist_create(const char *pathname, int delim);
void Wordlist_free(Wordlist *list);

#endif
