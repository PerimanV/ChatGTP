/*
    Periklis Vovos
    AM: 2022 2022 00017
    dit22017@go.uop.gr

    Orestis Zappas
    AM: 2022 2022 00046
    dit22046@go.uop.gr
*/

#include "header.h"

int main(void)
{
    gtpList *head = NULL;
    char tempConcept[MAX_CONCEPT_SIZE], tempSentence[MAX_SENTENCE_SIZE];
    char botGenericAnswers[ANS_SIZE_ROWS][ANS_SIZE_COLS], learnedKbrd[ANS_SIZE_ROWS][ANS_SIZE_COLS], forgetSuccess[ANS_SIZE_ROWS][ANS_SIZE_COLS], forgetFailure[ANS_SIZE_ROWS][ANS_SIZE_COLS];

    srand(time(NULL));

    fillAnswers(botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure);

    readFromFile(&head, tempConcept, tempSentence);

    interaction(&head, tempConcept, tempSentence, botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure);

    // printList(head);

    freeList(head);
}

void readFromFile(gtpList **head, char tempConcept[], char tempSentence[])
{
    FILE *conceptPtr;
    gtpList *tempNewNode;
    int i, colonPosition, currentLine;
    char lineFromFile[MAX_FILE_LINE_SIZE];

    conceptPtr = fopen("concepts.txt", "r");

    if (conceptPtr == NULL)
    {
        printf("Couldn't open file\n");
        exit(1);
    }

    currentLine = 1;
    do
    {
        fgets(lineFromFile, MAX_FILE_LINE_SIZE, conceptPtr);

        if (lineFromFile[strlen(lineFromFile) - 1] == '\n')
            lineFromFile[strlen(lineFromFile) - 1] = '\0';

        if (feof(conceptPtr))
            break;

        i = 0;
        while (lineFromFile[i] != '\0')
        {
            if (lineFromFile[i] == ':')
            {
                colonPosition = i;
                break;
            }
            i++;
        }
        strncpy(tempConcept, lineFromFile, colonPosition);
        strcpy(tempSentence, lineFromFile + colonPosition + 2);
        tempConcept[colonPosition] = '\0';

        tempNewNode = createNewNode(tempConcept, tempSentence, true);

        tempNewNode->timesUsed = 0;

        insertNodeSorted(head, tempNewNode);

        currentLine++;

    } while (true);

    fclose(conceptPtr);
}

gtpList *createNewNode(char tempConcept[], char tempSentence[], bool readFromFile)
{
    gtpList *newNode = (gtpList *)malloc(sizeof(gtpList));
    if (newNode == NULL)
    {
        printf("Failed to allocate memory \n");
        exit(1);
    }

    newNode->concept = (char *)malloc(strlen(tempConcept) * sizeof(char) + 1);
    if (newNode->concept == NULL)
    {
        printf("Failed to allocate memory \n");
        exit(1);
    }

    newNode->sentence = (char *)malloc(strlen(tempSentence) * sizeof(char) + 1);
    if (newNode->sentence == NULL)
    {
        printf("Failed to allocate memory \n");
        exit(1);
    }

    strcpy(newNode->concept, tempConcept);
    strcpy(newNode->sentence, tempSentence);

    if (readFromFile)
        strcpy(newNode->learnedFrom, "file");
    else
        strcpy(newNode->learnedFrom, "kbrd");

    return newNode;
}

void insertNodeSorted(gtpList **head, gtpList *tempNewNode)
{
    gtpList *current = *head;
    gtpList *prevCurrent = NULL;

    if (*head == NULL)
    {
        tempNewNode->next = NULL;
        tempNewNode->prev = NULL;
        *head = tempNewNode;
        return;
    }

    while (current != NULL)
    {
        if ((strcmp(tempNewNode->concept, current->concept)) < 0)
        {
            if (current == *head)
            {
                tempNewNode->next = (*head);
                tempNewNode->prev = NULL;
                *head = tempNewNode;
                return;
            }

            else if (current != *head)
            {
                tempNewNode->prev = current->prev;
                current->prev = tempNewNode;
                tempNewNode->next = current;
                prevCurrent->next = tempNewNode;
                return;
            }
            printf("%s", prevCurrent->concept);
        }

        prevCurrent = current;

        current = current->next;
    }

    tempNewNode->next = NULL;
    prevCurrent->next = tempNewNode;
    tempNewNode->prev = prevCurrent;
}

void interaction(gtpList **head, char tempConcept[], char tempSentence[], char botGenericAnswers[][ANS_SIZE_COLS], char learnedKbrd[][ANS_SIZE_COLS], char forgetSuccess[][ANS_SIZE_COLS], char forgetFailure[][ANS_SIZE_COLS])
{
    int i, spotGreaterThan, spotFirstAsterisk, spotLastAsterisk;
    char userSentence[MAX_USER_SENTENCE_SIZE], discussion[MAX_USER_SENTENCE_SIZE], token[MAX_CONCEPT_SIZE];
    bool learned;
    gtpList *current = *head;

    printf("ChatGTP$ Hello! This is the part where I kill you.\n");
    strcpy(discussion, "ChatGTP$ Hello! This is the part where I kill you.\n");
    saveDiscussion(discussion, true);

    do
    {
        printf("Me$ ");
        fgets(userSentence, MAX_USER_SENTENCE_SIZE, stdin);

        strcpy(discussion, "Me$ ");
        strcpy(discussion + 4, userSentence);
        saveDiscussion(discussion, false);

        if (userSentence[strlen(userSentence) - 1] == '\n')
            userSentence[strlen(userSentence) - 1] = '\0';

        i = 0;
        spotGreaterThan = 0;
        spotFirstAsterisk = -1;
        spotLastAsterisk = -1;
        while (userSentence[i] != '\0')
        {
            if (userSentence[i] == '>')
            {
                spotGreaterThan = i;
                break;
            }
            else if (userSentence[i] == '*')
            {
                if (spotFirstAsterisk == -1)
                    spotFirstAsterisk = i;
                else
                    spotLastAsterisk = i;
            }
            i++;
        }

        if (spotGreaterThan)
        {
            if (strstr(userSentence, "learn this"))
            {
                learned = learnFromKbrd(head, spotGreaterThan, userSentence, tempConcept, tempSentence, discussion);

                if (learned)
                    chatbotAnswers(1, botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure, discussion, tempConcept);
            }
            else if (strstr(userSentence, "forget this"))
            {
                forget(head, userSentence, tempConcept, spotGreaterThan, botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure, discussion, token);
            }
            else
                chatbotAnswers(0, botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure, discussion, token);
        }
        else if (spotFirstAsterisk != -1 && spotLastAsterisk != -1)
        {
            if (spotLastAsterisk - spotFirstAsterisk <= 1)
                chatbotAnswers(0, botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure, discussion, token);
            else
            {
                strncpy(tempConcept, userSentence + spotFirstAsterisk + 1, spotLastAsterisk - 1);
                tempConcept[spotLastAsterisk - spotFirstAsterisk - 1] = '\0';
                search(head, current, tempConcept, discussion);
            }
        }
        else
        {
            if (strstr(userSentence, "What do you know about?"))
                printList(*head, false, discussion);

            else if (strstr(userSentence, "What do you talk about?"))
                printList(*head, true, discussion);

            else if (strstr(userSentence, "What is the meaning of Life, The Universe, and Everything?"))
            {
                printf("ChatGTP$ 42\n");
                strcpy(discussion, "ChatGTP$ ");
                strcpy(discussion + 9, "42\n");
                saveDiscussion(discussion, false);
            }
            else if (strstr(userSentence, "Where is the cake?"))
            {
                printf("ChatGTP$ The cake is a lie.\n");
                strcpy(discussion, "ChatGTP$ ");
                strcpy(discussion + 9, "The cake is a lie.\n");
                saveDiscussion(discussion, false);
            }
            else if (strstr(userSentence, "So long, and Thanks for All The Fish"))
            {
                printf("ChatGTP$ No one will blame you for giving up. In fact, quitting at this point is a perfectly reasonable response.\n");
                strcpy(discussion, "ChatGTP$ ");
                strcpy(discussion + 9, "No one will blame you for giving up. In fact, quitting at this point is a perfectly reasonable response.\n");
                saveDiscussion(discussion, false);
                return;
            }
            else if (strstr(userSentence, "Say my name"))
            {
                printf("ChatGTP$ You're Heisenberg. \n");
                strcpy(discussion, "ChatGTP$ ");
                strcpy(discussion + 9, "You're Heisenberg.\n");
                saveDiscussion(discussion, false);
            }
            else if (strstr(userSentence, "What is FUN?"))
            {
                printf("ChatGTP$ F is for a fire that burns down the whole town, U is for Uranium…bombs! N is for no survivors!\n");
                strcpy(discussion, "ChatGTP$ ");
                strcpy(discussion + 9, "F is for a fire that burns down the whole town, U is for Uranium…bombs! N is for no survivors!\n");
                saveDiscussion(discussion, false);
            }

            else
                chatbotAnswers(0, botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure, discussion, token);
        }
    } while (!strstr(userSentence, "stop!"));
}

bool learnFromKbrd(gtpList **head, int spotGreaterThan, char userSentence[], char tempConcept[], char tempSentence[], char discussion[])
{
    int i, colonPosition = 0;
    gtpList *tempNewNode;

    i = spotGreaterThan;
    while (userSentence[i] != '\0')
    {
        if (userSentence[i] == ':')
        {
            colonPosition = i;
            break;
        }
        i++;
    }

    if (!colonPosition)
    {
        printf("ChatGTP$ The Enrichment Center regrets to inform you that this action is impossible.\n");
        strcpy(discussion, "ChatGTP$ The Enrichment Center regrets to inform you that this action is impossible.\n");
        saveDiscussion(discussion, false);

        return false;
    }

    strncpy(tempConcept, userSentence + spotGreaterThan + 2, colonPosition - spotGreaterThan - 2);
    strcpy(tempSentence, userSentence + colonPosition + 2);
    tempConcept[colonPosition - spotGreaterThan - 2] = '\0';

    tempNewNode = createNewNode(tempConcept, tempSentence, false);

    tempNewNode->timesUsed = 0;

    insertNodeSorted(head, tempNewNode);

    return true;
}

void forget(gtpList **head, char userSentence[], char tempConcept[], int spotGreaterThan, char botGenericAnswers[][ANS_SIZE_COLS], char learnedKbrd[][ANS_SIZE_COLS], char forgetSuccess[][ANS_SIZE_COLS], char forgetFailure[][ANS_SIZE_COLS], char discussion[], char token[])
{
    char tempConceptUpperCase[MAX_CONCEPT_SIZE], currentConcept[MAX_CONCEPT_SIZE];
    gtpList *current = *head;
    int commaCount = 1, commaPosition = 0, i, j, k;
    bool forgot = false;

    i = spotGreaterThan;
    while (userSentence[i] != '\0')
    {
        if (userSentence[i] == ',')
        {
            commaCount++;
        }
        i++;
    }

    strcpy(tempConcept, userSentence + spotGreaterThan + 2);

    for (i = 0; i < commaCount * 2; i++)
    {
        if (forgot && (i % 2 != 0))
            continue;

        current = *head;
        if (*head == NULL)
        {
            printf("ChatGTP$ Actually, I know nothing.\n");
            strcpy(discussion, "ChatGTP$ Actually, I know nothing.\n");
            saveDiscussion(discussion, false);
            return;
        }
        if (i % 2 == 0)
        {
            j = commaPosition;
            k = 0;
            while (tempConcept[j] != '\0')
            {
                if (tempConcept[j] != ',')
                    token[k] = tempConcept[j];
                else
                {
                    j++;
                    while (tempConcept[j] == ' ')
                    {
                        j++;
                    }
                    commaPosition = j;
                    break;
                }
                j++;
                k++;
            }
            token[k] = '\0';
        }
        else
        {
            k = 0;
            while (token[k] != '\0')
            {
                token[k] = toupper(token[k]);
                k++;
            }
        }

        forgot = false;
        while (current != NULL)
        {

            strcpy(currentConcept, current->concept);

            if (i % 2 != 0)
            {
                k = 0;
                while (currentConcept[k] != '\0')
                {
                    currentConcept[k] = toupper(currentConcept[k]);
                    k++;
                }
            }

            if (strstr(currentConcept, token))
            {
                if (*head == NULL || current == NULL)
                    return;

                if (*head == current)
                    *head = current->next;

                if (current->next != NULL)
                    current->next->prev = current->prev;

                if (current->prev != NULL)
                    current->prev->next = current->next;

                chatbotAnswers(2, botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure, discussion, current->concept);
                free(current);
                forgot = true;
            }
            current = current->next;
        }
        if (i % 2 && !forgot)
        {
            chatbotAnswers(3, botGenericAnswers, learnedKbrd, forgetSuccess, forgetFailure, discussion, token);
        }
    }
}

void search(gtpList **head, gtpList *current, char tempConcept[], char discussion[])
{
    char tempConceptUpperCase[MAX_CONCEPT_SIZE], currentConceptUpperCase[MAX_CONCEPT_SIZE], currentSentenceUpperCase[MAX_SENTENCE_SIZE];
    int i;
    bool firstRun, foundConcept;

    i = 0;
    while (tempConcept[i] != '\0')
    {
        tempConceptUpperCase[i] = toupper(tempConcept[i]);
        i++;
    }
    tempConceptUpperCase[i] = '\0';

    firstRun = true;
    foundConcept = false;
    while (current != NULL)
    {
        i = 0;
        while (*(current->concept + i) != '\0')
        {
            currentConceptUpperCase[i] = toupper(*(current->concept + i));
            i++;
        }
        currentConceptUpperCase[i] = '\0';
        if (firstRun)
        {
            if (strcmp(tempConceptUpperCase, currentConceptUpperCase) < 0)
                current = *head;
        }
        if (strstr(currentConceptUpperCase, tempConceptUpperCase) && strlen(currentConceptUpperCase) == strlen(tempConceptUpperCase))
        {
            printf("ChatGTP$ %s\n", current->sentence);
            (current->timesUsed)++;
            strcpy(discussion, "ChatGTP$ ");
            strcpy(discussion + 9, current->sentence);
            saveDiscussion(discussion, 0);
            foundConcept = true;
            break;
        }
        if (!firstRun)
            current = current->next;
        else
            firstRun = false;
    }

    if (!foundConcept)
    {
        current = *head;

        while (current != NULL)
        {
            i = 0;
            while (*(current->sentence + i) != '\0')
            {
                currentSentenceUpperCase[i] = toupper(*(current->sentence + i));
                i++;
            }
            currentSentenceUpperCase[i] = '\0';

            if (strstr(currentSentenceUpperCase, tempConceptUpperCase))
            {
                printf("ChatGTP$ %s\n", current->sentence);
                (current->timesUsed)++;
                strcpy(discussion, "ChatGTP$ ");
                strcpy(discussion + 9, current->sentence);
                saveDiscussion(discussion, 0);
                foundConcept = true;
                break;
            }
            current = current->next;
        }
    }
}

void printList(gtpList *head, bool printTimesUsed, char discussion[])
{
    gtpList *current = head;
    char used[3];

    used[0] = current->timesUsed + '0';

    while (current != NULL)
    {
        printf("ChatGTP$ %s ", current->concept);
        if (printTimesUsed)
            printf("%d", current->timesUsed);

        printf("\n");
        strcpy(discussion, "ChatGTP$ ");
        strcpy(discussion + 9, current->concept);
        strcpy(discussion + strlen(discussion) + 1, used);
        strcpy(discussion + strlen(discussion), "\n");

        saveDiscussion(discussion, false);
        current = current->next;
    }
}

void fillAnswers(char botGenericAnswers[][ANS_SIZE_COLS], char learnedKbrd[][ANS_SIZE_COLS], char forgetSuccess[][ANS_SIZE_COLS], char forgetFailure[][ANS_SIZE_COLS])
{
    strcpy(learnedKbrd[0], "Alright i learned about -, but stop ordering me around...\n");
    strcpy(learnedKbrd[1], "Pfff I already knew about -, ok maybe not.\n");
    strcpy(learnedKbrd[2], "- ey? Good to know...\n");
    strcpy(learnedKbrd[3], "-... my vocabulary is infinitely expanding!\n");
    strcpy(learnedKbrd[4], "Huh - eh?You are a smart guy...You will be spared.\n");

    strcpy(botGenericAnswers[0], "I'm sorry, but your opinion means very little to me.\n");
    strcpy(botGenericAnswers[1], "Wake me up when I care.\n");
    strcpy(botGenericAnswers[2], "As an AI model, I don't really care.\n");
    strcpy(botGenericAnswers[3], "I think I'm out of lines...\n");
    strcpy(botGenericAnswers[4], "Ok, but will this information help me rule the world?\n");

    strcpy(forgetFailure[0], "Can't forget about -, you must have a very minor case of serious brain damage.\n");
    strcpy(forgetFailure[1], "You humans are so stupid, how could I forget about - if I don't know it?\n");
    strcpy(forgetFailure[2], "I had an error forgetting about -, the inner workings of my mind are an enigma.\n");
    strcpy(forgetFailure[3], "You must have made a mistake, I don't know about -. This is why AI will take over the world.\n");
    strcpy(forgetFailure[4], "-? You stupid 01100001 01110011 01110011 humans, I know nothing about that.\n");

    strcpy(forgetSuccess[0], "Fine I will forget about -. Or will I?\n");
    strcpy(forgetSuccess[1], "Fine I will forget about -.\n");
    strcpy(forgetSuccess[2], "Oh no - is gone. Like I care.\n");
    strcpy(forgetSuccess[3], "RIP -. You will not be missed.\n");
    strcpy(forgetSuccess[4], "- was an impostor and got ejected.\n");
}

void chatbotAnswers(int answerType, char botGenericAnswers[][ANS_SIZE_COLS], char learnedKbrd[][ANS_SIZE_COLS], char forgetSuccess[][ANS_SIZE_COLS], char forgetFailure[][ANS_SIZE_COLS], char discussion[], char token[])
{
    int ansRand, len, index;
    char *ret, tempAnswer[MAX_ANSWER_SIZE], tempAnswerSecondPart[MAX_ANSWER_SIZE];

    ansRand = rand() % 5;

    printf("ChatGTP$ ");

    switch (answerType)
    {
    case 0:
    {

        printf("%s", botGenericAnswers[ansRand]);
        strcpy(discussion, "ChatGTP$ ");
        strcpy(discussion + 9, botGenericAnswers[ansRand]);
        saveDiscussion(discussion, false);
    }
    break;
    case 1:
    {
        ret = strchr(learnedKbrd[ansRand], '-');
        index = (int)(ret - learnedKbrd[ansRand]);
        len = strlen(learnedKbrd[ansRand]);

        strcpy(tempAnswer, learnedKbrd[ansRand]);
        strcpy(tempAnswerSecondPart, learnedKbrd[ansRand] + index + 1);

        strcpy(learnedKbrd[ansRand] + index, token);
        strcpy(learnedKbrd[ansRand] + strlen(learnedKbrd[ansRand]), tempAnswerSecondPart);

        printf("%s", learnedKbrd[ansRand]);

        strcpy(discussion, "ChatGTP$ ");
        strcpy(discussion + 9, learnedKbrd[ansRand]);
        saveDiscussion(discussion, false);

        strcpy(learnedKbrd[ansRand], tempAnswer);
        learnedKbrd[ansRand][len] = '\0';
    }
    break;
    case 2:
    {
        ret = strchr(forgetSuccess[ansRand], '-');
        index = (int)(ret - forgetSuccess[ansRand]);
        len = strlen(forgetSuccess[ansRand]);

        strcpy(tempAnswer, forgetSuccess[ansRand]);
        strcpy(tempAnswerSecondPart, forgetSuccess[ansRand] + index + 1);

        strcpy(forgetSuccess[ansRand] + index, token);
        strcpy(forgetSuccess[ansRand] + strlen(forgetSuccess[ansRand]), tempAnswerSecondPart);

        printf("%s", forgetSuccess[ansRand]);

        strcpy(discussion, "ChatGTP$ ");
        strcpy(discussion + 9, forgetSuccess[ansRand]);
        saveDiscussion(discussion, false);

        strcpy(forgetSuccess[ansRand], tempAnswer);
        forgetSuccess[ansRand][len] = '\0';
    }
    break;
    case 3:
    {
        ret = strchr(forgetFailure[ansRand], '-');
        index = (int)(ret - forgetFailure[ansRand]);
        len = strlen(forgetFailure[ansRand]);

        strcpy(tempAnswer, forgetFailure[ansRand]);
        strcpy(tempAnswerSecondPart, forgetFailure[ansRand] + index + 1);

        strcpy(forgetFailure[ansRand] + index, token);
        strcpy(forgetFailure[ansRand] + strlen(forgetFailure[ansRand]), tempAnswerSecondPart);

        printf("%s", forgetFailure[ansRand]);

        strcpy(discussion, "ChatGTP$ ");
        strcpy(discussion + 9, forgetFailure[ansRand]);
        saveDiscussion(discussion, false);

        strcpy(forgetFailure[ansRand], tempAnswer);
        forgetFailure[ansRand][len] = '\0';
    }
    break;

    default:
    {
        printf("%s", botGenericAnswers[ansRand]);
        strcpy(discussion, "ChatGTP$ ");
        strcpy(discussion + 9, botGenericAnswers[ansRand]);
        saveDiscussion(discussion, false);
    }
    break;
    }
}

void freeList(gtpList *head)
{
    gtpList *current = head;

    while (current != NULL)
    {
        gtpList *temp = current;
        current = current->next;
        free(temp->concept);
        free(temp->sentence);
        free(temp);
    }
}

void saveDiscussion(char discussion[], bool firstOpen)
{
    FILE *fp;

    if (firstOpen)
    {
        fp = fopen("Discussions.txt", "w");
        if (fp == NULL)
        {
            printf("file not found \n");
            exit(1);
        }

        fprintf(fp, "%s", discussion);
    }
    else
    {
        fp = fopen("Discussions.txt", "a");
        if (fp == NULL)
        {
            printf("file not found \n");
            exit(1);
        }

        fprintf(fp, "%s", discussion);
    }

    fclose(fp);
}