// Declares a dictionary's functionality

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>

// Maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45
#define N 27

typedef struct table {
    struct table *nexts[N];
    bool complete_word;
}table;

// Prototypes
bool check(const char *word);
unsigned int hash(const char letter);
bool load(const char *dictionary);
unsigned int size(void);
bool unload(void);
bool free_dict(table *dict);
unsigned int size_dict(table *dict);

#endif // DICTIONARY_H
