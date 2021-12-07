#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

typedef struct {
    long val;
    int requestNumber;
    struct Request *next;
} Request;

typedef struct {
    Request *head;
    Request *tail;
} ListOfRequests;

void push(ListOfRequests *listOfRequests, long sec, int line) {
    Request *new = (Request *)calloc(1, sizeof(Request));
    new->val = sec;
    new->requestNumber = line;
    new->next = NULL;


    if (listOfRequests->head == NULL) {
        listOfRequests->tail = new;
        listOfRequests->head = new;

    }
    else {
        listOfRequests->tail->next = (struct Request *) new;
        listOfRequests->tail = new;
    }
}

void pop(ListOfRequests *listOfRequests) {
    listOfRequests->head = (Request *) listOfRequests->head->next;
}

int main () {
    FILE *f;
    f = fopen("C:\\Users\\ulyag\\CLionProjects\\proga3\\cmake-build-debug\\access_log_Jul95.txt", "r");
    if (f == NULL) printf("faila net, sosatb");

    int new_symbol, i = 0, count_request = 0, line = 0, capacity = 1, max_count, start, end;
    ListOfRequests listOfRequests = {NULL, NULL};
    printf("enter the desired interval:\n");
    int period = 1;
    scanf("%d", &period);

    printf("list of requests that ended with a 5xx error:\n");

    while ((new_symbol = getc(f)) != EOF) {
        char *str = (char*)malloc(sizeof(char));

        while (new_symbol != '\n') {
            str[i++] = (char)new_symbol;
            if (i >= capacity) {
                capacity *= 2;
                str = (char *) realloc(str, sizeof(char) * capacity);
            }
            new_symbol = getc(f);
        }

        str[i] = '\0';
        //new_symbol = '\n', в str сейчас лежит одна строка, в i  лежит длина строки
        i = 0;
        capacity = 1;

        char *request, *time, *status;

        strtok(str, "[");
        time = strtok(NULL, "]");
        //time - указатель на строку времени
        strtok(NULL, "\"");

        request = strtok(NULL, "\"");
        //request - указатель на строку запроса

        status = strtok(NULL, " ");
        //status - указатель на строку со статусом запроса

        if (status[0] == '5') puts(request);


        struct tm cur;
        char month[12][4] =
                {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        cur.tm_mday = atoi(strtok(time, "/"));

        char *tmp = strtok(NULL, "/");
        for (int j = 0; j < 12; j++) if (strcmp(tmp, month[j]) == 0) cur.tm_mon = j;

        cur.tm_year = atoi(strtok(NULL, ":")) - 1900;
        cur.tm_hour = atoi(strtok(NULL, ":"));
        cur.tm_min = atoi(strtok(NULL, ":"));
        cur.tm_sec = atoi(strtok(NULL, " "));

        long sec = (long)mktime(&cur);

        push(&listOfRequests, sec, line);
        while ((sec - listOfRequests.head->val) >= period)
            pop(&listOfRequests);

        int numberOfRequests = line - (listOfRequests.head->requestNumber) + 1;
        if (numberOfRequests > max_count)
        {
            max_count = numberOfRequests;
            start = listOfRequests.head->val;
            end = sec;

        }
        free(str);
        line++;
    }

    printf("\nThe largest amount of requests in period %d second: %d\nTime interval: from %d second to %d second", period, max_count, start, end);
    fclose(f);
    return 0;
}
