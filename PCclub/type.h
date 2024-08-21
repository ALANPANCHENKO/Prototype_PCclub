#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TIME_LENGTH 6

typedef struct
{
    char *name;
    int table;
    char time_of_entry[MAX_TIME_LENGTH];
} Client;

typedef struct
{
    int number;
    int profit;
    int occupied_time;
    char *client;
} Table;

typedef struct
{
    char time[MAX_TIME_LENGTH];
    int id;
    char *data;
} Event;

extern Client *clients;
extern Table *tables;
extern int num_tables;
extern char start_time[MAX_TIME_LENGTH], end_time[MAX_TIME_LENGTH];
extern int hour_rate;
extern int num_clients;
extern int queue_size;
extern Client **queue;
extern Event *events;
extern int num_events;

#endif
