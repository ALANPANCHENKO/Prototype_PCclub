#include "type.h"

Client clients[MAX_CLIENTS];
Table *tables;
int num_tables;
char start_time[MAX_TIME_LENGTH], end_time[MAX_TIME_LENGTH];
int hour_rate;
int num_clients;
int queue_size;
Client *queue[MAX_CLIENTS];
Event events[MAX_EVENTS];
int num_events;
