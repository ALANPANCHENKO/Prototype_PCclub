#include "type.h"

Client *clients;
Table *tables;
int num_tables = 0;
char start_time[MAX_TIME_LENGTH], end_time[MAX_TIME_LENGTH];
int hour_rate;
int num_clients = 0;
Client **queue;
int queue_size = 0;
Event *events;
int num_events = 0;

