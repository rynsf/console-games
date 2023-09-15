#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct qNode {
    char *ask;
    struct qNode *yes;
    struct qNode *no;
} qNode;

int freemem(qNode* ptr)
{
    if(ptr == NULL)
    {
        return 0;
    }
    freemem(ptr -> yes);
    freemem(ptr -> no);
    free(ptr -> ask);
    free(ptr);
    return 0;
}

char *readline(FILE *file)
{
    size_t size  = 80;
    size_t curr  = 0;
    char *buffer = malloc(size);
    while(fgets(buffer + curr, size - curr, file))
    {
        char* nextln = strchr(buffer + curr, '\n');
        if(nextln)
        {
            *nextln = '\0';
            if(buffer[0] == '\0')
            {
                free(buffer);
                return NULL;
            }
            else
            {
                return buffer;
            }
        }
        curr = size - 1;
        size *= 2;
        char *tmp = realloc(buffer, size);
        if(tmp == NULL)
        {
            //TODO Error handling
        }
        buffer = tmp;
    }
    free(buffer);
    return NULL;
}

char getch()
{
    char inpt;
    reinput:
    inpt = getchar();
    if(inpt != 'Y' && inpt != 'y' && inpt != 'N' && inpt != 'n')
        goto reinput;
    while(getchar() != '\n');
    return inpt;
}

qNode* newNode(char* str)
{
    qNode* root = malloc(sizeof(qNode));
    root -> ask = str;
    root -> yes = NULL;
    root -> no = NULL;
    return root;
}

int serialize(qNode* root, FILE* fp)
{
    if(root == NULL)
    {
        fprintf(fp, "\n");
        return 0;
    }
    fprintf(fp, "%s\n", root -> ask);
    serialize(root -> yes, fp);
    serialize(root -> no, fp);
    return 0;
}

int deSerialize(qNode** root, FILE* fp)
{
    char* str = readline(fp);
    if(str == NULL)
    {
        return 0;
    }

    *root = newNode(str);
    deSerialize(&((*root) -> yes), fp);
    deSerialize(&((*root) -> no), fp);
    return 0;
}

int print(qNode* node) //TODO print known animals
{
    if(node == NULL)
        return 0;

    print(node -> no);
    print(node -> yes);
    printf("%s\n", node -> ask);
    return 0;
}

int main()
{
    qNode* q1 = NULL;
    FILE* fp = fopen("knownAnimals.txt", "r");
    if(fp == NULL)
    {
        printf("Failed to open File\n");
        return -1;
    }
    deSerialize(&q1, fp);
    fclose(fp);

    char ans;
    qNode *q = q1;
    qNode *qParrent;
    printf("Computer: Are you Thinking of an animal (Y or N)? \n");
    printf("You: ");
    char inpt = getch();
    while(inpt == 'Y' || inpt == 'y')
    {
        while(1)
        {
            if(q -> yes == NULL && q -> no == NULL)
            {
                printf("Computer: Are you thinking of %s?\n", q -> ask);
                printf("You: ");
                char wasCorrect = getch();
                if(wasCorrect == 'Y' || wasCorrect == 'y')
                {
                    printf("Computer: Nice! I won!\n");
                    break;
                }
                else
                {
                    qNode *tempNode;
                    qNode *newQ = malloc(sizeof(qNode));
                    newQ -> yes = NULL;
                    newQ -> no = NULL;
                    qNode *newA = malloc(sizeof(qNode));
                    newA -> yes = NULL;
                    newA -> no = NULL;
                    printf("Computer: I can't think of any such animal right now...\n");
                    printf("Computer: Which animal were you thinking of?\n");
                    printf("You: ");
                    newA -> ask = readline(stdin);
                    printf("Computer: Please tell me a question that could differentiate %s from %s.\n", newA -> ask, q -> ask);
                    printf("You: ");
                    newQ -> ask = readline(stdin);
                    printf("Computer: What should the answer for this question be for %s?\n", newA -> ask);
                    printf("You: ");
                    ans = getch();
                    if(ans == 'Y' || ans == 'y')
                    {
                        if(inpt == 'Y' || inpt == 'y')
                        {
                            tempNode = qParrent -> yes;
                            qParrent -> yes = newQ;
                            newQ -> yes = newA;
                            newQ -> no = tempNode;
                            break;
                        }
                        else
                        {
                            tempNode = qParrent -> no;
                            qParrent -> no = newQ;
                            newQ -> yes = newA;
                            newQ -> no = tempNode;
                            break;
                        }
                    }
                    else
                    {
                        if(inpt == 'Y' || inpt == 'y')
                        {
                            tempNode = qParrent -> yes;
                            qParrent -> yes = newQ;
                            newQ -> yes = tempNode;
                            newQ -> no = newA;
                            break;
                        }
                        else
                        {
                            tempNode = qParrent -> no;
                            qParrent -> no = newQ;
                            newQ -> yes = tempNode;
                            newQ -> no = newA;
                            break;
                        }
                    }
                }
            }
            else
            {
                printf("Computer: %s\n", q -> ask);
                printf("You: ");
                inpt = getch();
                if(inpt == 'Y' || inpt == 'y')
                {
                    qParrent = q;
                    q = q -> yes;
                }
                else
                {
                    qParrent = q;
                    q = q -> no;
                }
            }
        }
        printf("Computer: Are you Thinking of an animal (Y or N)? \n");
        printf("You: ");
        inpt = getch();
        q = q1;
    }
    printf("Computer: Exiting, Do you want to save currently know animals?\n");
    printf("You: ");
    inpt = getch();
    if(inpt == 'Y' || inpt == 'y')
    {
        FILE* fp = fopen("knownAnimals.txt", "w+");
        if(fp == NULL)
        {
            printf("Error: Failed to open File\n");
            return -1;
        }
        serialize(q1, fp);
        fclose(fp);
        printf("Computer: Known animals saved successfully.\n");
    }
    else
    {
        printf("Computer: Known animals not saved.\n");
    }
    printf("Computer: Good Bye!\n");
    freemem(q1);
    return 0;
}
