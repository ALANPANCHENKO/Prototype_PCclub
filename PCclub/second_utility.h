#ifndef UTIL_H
#define UTIL_H

#include "type.h"

int is_valid_time_format(const char *time_str);
int is_valid_format(const int *valid_format);
int time_to_minutes(const char *time_str);
void minutes_to_time(int minutes, char *time_str);
Client* find_client(const char *name);
void remove_client(Client *client);
void print_error(const char *time, int id, const char *message);
void print_table_summary();

#endif // UTIL_H
