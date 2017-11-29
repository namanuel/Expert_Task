//
// Created by es on 24.10.17.
//
#define WORDBUFFSIZE 15
#define KEY_MAX_LENGTH (256)
#define KEY_PREFIX ("somekey")
#define KEY_COUNT (1024*1024)

#include "helper.h"
#include "hashmap.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct word_struct
{ //struct for words and counts
    char key_string[KEY_MAX_LENGTH];
    int number;
} word_struct;

struct statistic {
    //struct for words and counts to make the statistic
    char word[WORDBUFFSIZE];
    int wordcount;
};

int countWordsinfile(FILE *file) {
    //WORDS COUNT IN FILE
    int counter = 0;
    char word[WORDBUFFSIZE + 1];
    while (fscanf(file, "%s", word) == 1) {
        counter++;
    }
    return counter;
}

int decodefile(uint16_t character) {
    //DECODER
    uint16_t actual_character;
    int key;
    for (key = 0; key <= 8; key++) {
        int calculated_number = 1 << key;

        actual_character = character / calculated_number;

        if ((actual_character > 64 && actual_character < 91)
            || actual_character == 32)
            break;
    }

    return actual_character;
}

int compare(const void *a, const void *b) {

    const char *ia = (const char *) a;
    const char *ib = (const char *) b;
    return strcmp(ia, ib);
}

int statisticcompare(const void *a, const void *b) {
    struct statistic *A;
    A = (struct statistic *) a;
    struct statistic *B;
    B = (struct statistic *) b;
    if (A->wordcount > B->wordcount) {
        return -1;
    } else if (A->wordcount == B->wordcount) {
        return 0;
    } else {
        return 1;
    }
}

int statisticFileWriter(char *output, char *statistic, int count) {
    char buff[WORDBUFFSIZE];
    char key_string[count][WORDBUFFSIZE];
    FILE *f = fopen(output, "r");
    FILE *s = fopen(statistic, "w");
    char storage[WORDBUFFSIZE];
    struct statistic statisticarray[count];
    char helper[WORDBUFFSIZE] = {0};
    int index;
    map_t mymap;
    word_struct* value;
    //HASHMAP: https://github.com/petewarden/c_hashmap
    mymap = hashmap_new();

    for (int i = 0; fscanf(f, "%s", buff) == 1; i++) {
        strcpy(key_string[i], buff);
    }

    qsort(key_string, count, WORDBUFFSIZE, compare);

    /* First, populate the hash map with ascending values */
    for (index=0; index<count; index+=1)
    {
        /* Store the key string along side the numerical value so we can free it later */
        value = malloc(sizeof(word_struct));
        snprintf(value->key_string, KEY_MAX_LENGTH, "%s", key_string[index]);
        value->number = 0;
        hashmap_put(mymap, value->key_string, value);
    }
    /* Now, check all of the expected values are there */
    for (index=0; index<count; index+=1)
    {
        snprintf(storage, KEY_MAX_LENGTH, "%s", key_string[index]);
        hashmap_get(mymap, storage, (void**)(&value));
        value->number++;
        hashmap_put(mymap, value->key_string, value);
    }

    for (index=0; index<count; index+=1)
    {
        snprintf(storage, KEY_MAX_LENGTH, "%s", key_string[index]);
        hashmap_get(mymap, storage, (void**)(&value));
        strcpy(statisticarray[index].word, value->key_string);
        statisticarray[index].wordcount = value->number;
    }
    //SORT to make the words in the right order (most words - less words)
    qsort(statisticarray, count, sizeof(struct statistic), statisticcompare);

    for (int i = 0; i< count; i++){

        //print online 1 time the word into the statistic file
        if (strcmp(helper, statisticarray[i].word)!=0){
            strcpy(helper, statisticarray[i].word);
            fprintf(s, "\n %s : %d", statisticarray[i].word, statisticarray[i].wordcount);
        }
    }
    fclose(s);
    fclose(f);
    return 0;
}