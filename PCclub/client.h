#ifndef CLIENT_H
#define CLIENT_H

#include "type.h"

// void print_table_summary();
void client_arrival(Event event);
void client_exit_if_queue_full(Event event);
void client_wait(Event event);
void client_seat(Event event);
void seat_next_client_in_queue(Table *table, const char *current_time);
void client_leaves(Event event);
void end_of_day();

#endif