// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

table DICTIONARY;

bool check(const char *word)
{
    unsigned int word_lenght = strlen(word);
    char word_copy[word_lenght+1];


    for (int i = 0; i < word_lenght; i++)
    {
        word_copy[i] = tolower(word[i]);
        if (i>=LENGTH) {
            printf("there is a word bigger then max lenght being checked");
        }
    }

    word_copy[word_lenght] = '\0';

    table *current = &DICTIONARY;
    for (int i = 0; i <= word_lenght; i++)
    {
        if (word_copy[i] == '\0') {
            return current->complete_word;
        }
        else {
            unsigned int index = hash(word_copy[i]);
            if (current->nexts[index] == NULL)
            {
                // if (current->complete_word[index]) { // this was wrong because i forgot that even it it is the last letter it will still have a seperete pointer and still go to another array, only in it there wont be any pointer to another letter level
                //     continue;
                // }
                // else
                return false;
            }
            else current = current->nexts[index];
        }
    }
    return false;
}

unsigned int hash(const char letter)
{
    return (letter == '\'') ? 26 : toupper(letter) - 'A';
}

bool load(const char *dictionary)
{
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        return false;
    }

    char word[LENGTH+1];

    for (int i = 0; i < N; i++) {
        DICTIONARY.nexts[i] = NULL;
        DICTIONARY.complete_word = false;
    }

    while (fscanf(file, "%s", word) != EOF)
    {
        unsigned int word_lenght = strlen(word);
        for (int i = 0; i < word_lenght; i++)
        {
            word[i] = tolower(word[i]);
            if (i>=LENGTH) {
                printf("there is a word bigger then max lenght");
            }
        }


        table *current = &DICTIONARY;
        for (int i = 0; i <= word_lenght; i++)
        {
            if (word[i] == '\0') {
                current->complete_word = true;
            }
            else {
                unsigned int index = hash(word[i]);
                if (current->nexts[index] == NULL)
                {
                    current->nexts[index] = malloc(sizeof(table));
                    if (current->nexts[index] == NULL)
                    {
                        fclose(file);
                        return false;
                    }

                    for (int j = 0; j < N; j++)
                    {
                        current->nexts[index]->nexts[j] = NULL;
                        current->nexts[index]->complete_word = false;
                    }
                }
                current = current->nexts[index];
            }
        }
    }

    fclose(file);
    return true;
}

unsigned int size(void)
{
    return size_dict(&DICTIONARY);
}

unsigned int size_dict(table *dict) {
    unsigned int size_sum = 0;
    for (int i = 0; i < N; i++) {
        if (dict->nexts[i] != NULL) {
            size_sum += size_dict(dict->nexts[i]);
            // if (dict->complete_word) {
            //     size_sum++;
            // }
        }
        // else if (dict->complete_word) return 1;
    }
    if (dict->complete_word) size_sum++; // with the new way of signaling that the word is misspelled the sizing is easier, and just the structure is more conventional
    return size_sum;
}

bool unload(void)
{
    bool result = true;
    for (int i = 0; i < N; i++) {
        if (DICTIONARY.nexts[i] != NULL) {
            result &= free_dict(DICTIONARY.nexts[i]);
        }
    }
    return result;
}

bool free_dict(table *dict) {
    bool result = true;
    for (int i = 0; i < N; i++) {
        if (dict->nexts[i] != NULL) {
            result &= free_dict(dict->nexts[i]);
        }
    }
    free(dict);
    return result;
}