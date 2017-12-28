#include "wordlist.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>


Wordlist *Wordlist_create(const char *pathname, int delim)
{
    FILE *file;
    Wordlist *list;
    Darray *words;
    char *wordBuf;
    char *word;
    ssize_t wordSize;
    size_t maxWord;

    maxWord = WORDLIST_WORD_SIZE;

    file = fopen(pathname, "r");
    if (file == NULL) {
        errno = ENOENT;
        return NULL;
    }

    list = calloc(1, sizeof(Wordlist));
    if (list == NULL) {
        errno = ENOMEM;
        fclose(file);
        return NULL;
    }

    words = Darray_create(sizeof(void *), WORDLIST_INITIAL_SIZE);
    if (words == NULL) {
        errno = ENOMEM;
        free(list);
        fclose(file);
        return NULL;
    }

    wordBuf = calloc(1, maxWord);

    while ((wordSize = getdelim(&wordBuf, &maxWord, delim, file)) > 0) {
        word = calloc(1, wordSize);
        if (word == NULL) {
            wordSize = -1;
            break;
        }

        strncpy(word, wordBuf, wordSize - 1);

        Darray_push(words, word);
    }

    list->words = words;

    fclose(file);

    return list;
}


void Wordlist_free(Wordlist *list)
{
    Darray_clear_destroy(list->words);
    free(list);
}
