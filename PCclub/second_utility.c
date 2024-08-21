#include "type.h"
#include "second_utility.h"

int is_valid_time_format(const char *time_str)
{
    if (strlen(time_str) != 5)
    {
        return 0;
    }
    if (time_str[2] != ':')
    {
        return 0;
    }
    for (int i = 0; i < 5; i++)
    {
        if (i != 2 && (time_str[i] < '0' || time_str[i] > '9'))
        {
            return 0;
        }
    }

    int hours, minutes;
    sscanf(time_str, "%2d:%2d", &hours, &minutes);
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59)
    {
        return 0;
    }
    return 1;
}

int is_valid_format(const int *valid_format)
{
    int value = *valid_format;
    if (value <= 0)
    {
        return 0;
    }
    return 1;
}

int time_to_minutes(const char *time_str) {
    int hours, minutes;
    sscanf(time_str, "%d:%d", &hours, &minutes);
    return hours * 60 + minutes;
}

void minutes_to_time(int minutes, char *time_str) {
    int hours = minutes / 60;
    minutes %= 60;
    sprintf(time_str, "%02d:%02d", hours, minutes);
}

Client* find_client(const char *name) {
    for (int i = 0; i < num_clients; i++) {
        if (strcmp(clients[i].name, name) == 0) {
            return &clients[i];
        }
    }
    return NULL;
}

void remove_client(Client *client) {
    int index = -1;
    for (int i = 0; i < num_clients; i++) {
        if (&clients[i] == client) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        for (int i = index; i < num_clients - 1; i++) {
            clients[i] = clients[i + 1];
        }
        num_clients--;
    }
}

void print_error(const char *time, int id, const char *message) {
    printf("%s %d %s\n", time, id, message);
}

void print_table_summary() {
    printf("%s\n", end_time);
    char time_str[6];
    for (int i = 0; i < num_tables; i++) {
        minutes_to_time(tables[i].occupied_time, time_str);
        printf("%d %d %s\n", tables[i].number, tables[i].profit, time_str);
    }
}
