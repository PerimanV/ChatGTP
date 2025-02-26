/*
    Periklis Vovos
    AM: 2022 2022 00017
    dit22017@go.uop.gr

    Orestis Zappas
    AM: 2022 2022 00046
    dit22046@go.uop.gr
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define MAX_FILE_LINE_SIZE 1250
#define MAX_CONCEPT_SIZE 300
#define MAX_SENTENCE_SIZE 1000
#define MAX_USER_SENTENCE_SIZE 1250
#define MAX_USER_COMMAND_SIZE 15
#define MAX_ANSWER_SIZE 400
#define ANS_SIZE_ROWS 5
#define ANS_SIZE_COLS 400
#define TYPE_SIZE 20

typedef struct gList
{
    char *concept;       // the concept learned
    char *sentence;      // the sentence associated with the concept
    int timesUsed;       // no of times the concept was used to an answer
    char learnedFrom[5]; // learned either from "file" or "kbrd"
    struct gList *next;
    struct gList *prev;
} gtpList;

void readFromFile(gtpList **head, char tempConcept[], char tempSentence[]);
gtpList *createNewNode(char tempConcept[], char tempSentence[], bool readFromFile);
void insertNodeSorted(gtpList **head, gtpList *tempNewNode);
void printList(gtpList *head, bool printTimesUsed, char discussion[]);
void freeList(gtpList *head);
void interaction(gtpList **head, char tempConcept[], char tempSentence[], char botGenericAnswers[][ANS_SIZE_COLS], char learnedKbrd[][ANS_SIZE_COLS], char forgetSuccess[][ANS_SIZE_COLS], char forgetFailure[][ANS_SIZE_COLS]);
void search(gtpList **head, gtpList *current, char tempConcept[], char discussion[]);
bool learnFromKbrd(gtpList **head, int spotGreaterThan, char userSentence[], char tempConcept[], char tempSentence[], char discussion[]);
void forget(gtpList **head, char userSentence[], char tempConcept[], int spotGreaterThan, char botGenericAnswers[][ANS_SIZE_COLS], char learnedKbrd[][ANS_SIZE_COLS], char forgetSuccess[][ANS_SIZE_COLS], char forgetFailure[][ANS_SIZE_COLS], char discussion[], char token[]);
void chatbotAnswers(int answerType, char botGenericAnswers[][ANS_SIZE_COLS], char learnedKbrd[][ANS_SIZE_COLS], char forgetSuccess[][ANS_SIZE_COLS], char forgetFailure[][ANS_SIZE_COLS], char discussion[], char token[]);
void fillAnswers(char botGenericAnswers[][ANS_SIZE_COLS], char learnedKbrd[][ANS_SIZE_COLS], char forgetSuccess[][ANS_SIZE_COLS], char forgetFailure[][ANS_SIZE_COLS]);
void saveDiscussion(char discussion[], bool firstOpen);
