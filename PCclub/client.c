#include "type.h"
#include "client.h"
#include "second_utility.h"

// void print_table_summary() {
//     printf("%s\n", end_time);
//     char time_str[6];
//     for (int i = 0; i < num_tables; i++) {
//         minutes_to_time(tables[i].occupied_time, time_str);
//         printf("%d %d %s\n", tables[i].number, tables[i].profit, time_str);
//     }
// }

void client_arrival(Event event)
{
    char *client_name = malloc(strlen(event.data) + 1);
    sscanf(event.data, "%s", client_name);
    Client *visitor = find_client(client_name);
    if (visitor == NULL)
    {
        if (time_to_minutes(event.time) < time_to_minutes(start_time) || time_to_minutes(event.time) > time_to_minutes(end_time))
        {
            print_error(event.time, 13, "NotOpenYet");
            free(client_name);
            return;
        }

        clients = realloc(clients, (num_clients + 1) * sizeof(Client));
        clients[num_clients].name = client_name;
        strcpy(clients[num_clients].time_of_entry, event.time);
        clients[num_clients].table = -1;
        num_clients++;
    }
    else
    {
        printf("The client is in the computer club\n");
        free(client_name);
        return;
    }
}

void client_exit_if_queue_full(Event event)
{
    char *client_name = malloc(strlen(event.data) + 1);
    sscanf(event.data, "%s", client_name);
    Client *visitor = find_client(client_name);
    if (visitor == NULL)
    {
        print_error(event.time, 13, "ClientUnknow");
        free(client_name);
        return;
    }
    printf("%s %d %s\n", event.time, event.id, client_name);
    remove_client(visitor);
    free(client_name);
}

void client_wait(Event event)
{
    char *client_name = malloc(strlen(event.data) + 1);
    sscanf(event.data, "%s", client_name);
    Client *visitor = find_client(client_name);

    if (time_to_minutes(event.time) < time_to_minutes(start_time) || time_to_minutes(event.time) > time_to_minutes(end_time))
    {
        printf("Invalid time event %s\n", event.time);
        free(client_name);
        exit(1);
    }

    if (visitor == NULL)
    {
        print_error(event.time, 13, "ClientUnknow");
        free(client_name);
        return;
    }

    int free_tables = 0;
    for (int i = 0; i < num_tables; i++)
    {
        if (!tables[i].client)
        {
            free_tables++;
        }
    }

    if (free_tables > 0)
    {
        print_error(event.time, 13, "ICanWaitNoLonger!");
        free(client_name);
        return;
    }

    if (queue_size >= num_tables)
    {
        event.id = 11;
        client_exit_if_queue_full(event);
    }
    else
    {
        queue = realloc(queue, (queue_size + 1) * sizeof(Client *));
        queue[queue_size++] = visitor;
    }
    free(client_name);
}

void client_seat(Event event)
{
    char *client_name = malloc(strlen(event.data) + 1);
    int table_number;
    sscanf(event.data, "%s %d", client_name, &table_number);

    Client *visitor = find_client(client_name);

    if (time_to_minutes(event.time) < time_to_minutes(start_time) || time_to_minutes(event.time) > time_to_minutes(end_time))
    {
        printf("Invalid time event %s\n", event.time);
        free(client_name);
        exit(1);
    }

    if (visitor == NULL)
    {
        print_error(event.time, 13, "ClientUnknow");
        free(client_name);
        return;
    }

    if (table_number < 1 || table_number > num_tables)
    {
        print_error(event.time, 13, "PlaceIsBusy");
        free(client_name);
        return;
    }
    Table *table = &tables[table_number - 1];
    if (table->client && strcmp(table->client, client_name) != 0)
    {
        print_error(event.time, 13, "PlaceIsBusy");
        free(client_name);
        return;
    }

    if (visitor->table != -1)
    {
        Table *old_table = &tables[visitor->table - 1];
        old_table->client = NULL;
    }

    table->client = visitor->name;
    strcpy(visitor->time_of_entry, event.time);
    visitor->table = table_number;

    for (int i = 0; i < queue_size; i++)
    {
        if (strcmp(queue[i]->name, client_name) == 0)
        {
            memmove(queue + i, queue + i + 1, (queue_size - i - 1) * sizeof(Client *));
            queue_size--;
            queue = realloc(queue, queue_size * sizeof(Client *));
            break;
        }
    }
    free(client_name);
}

void seat_next_client_in_queue(Table *table, const char *current_time)
{
    if (queue_size > 0)
    {
        Client *next_client = queue[0];
        memmove(queue, queue + 1, (--queue_size) * sizeof(Client *));
        queue = realloc(queue, queue_size * sizeof(Client *));

        Event seat_event;
        strcpy(seat_event.time, current_time);
        seat_event.id = 12;
        seat_event.data = malloc(256);
        snprintf(seat_event.data, 256, "%s %d", next_client->name, table->number);
        printf("%s %d %s %d\n", seat_event.time, seat_event.id, next_client->name, table->number);
        client_seat(seat_event);
        free(seat_event.data);
    }
}

void client_leaves(Event event)
{
    char *client_name = malloc(strlen(event.data) + 1);
    sscanf(event.data, "%s", client_name);

    Client *visitor = find_client(client_name);

    if (time_to_minutes(event.time) < time_to_minutes(start_time) || time_to_minutes(event.time) > time_to_minutes(end_time))
    {
        printf("Invalid time event %s\n", event.time);
        free(client_name);
        exit(1);
    }

    if (visitor == NULL)
    {
        print_error(event.time, 13, "ClientUnknow");
        free(client_name);
        return;
    }

    int table_number = visitor->table;
    if (table_number != -1)
    {
        Table *table = &tables[table_number - 1];
        table->client = NULL;
        visitor->table = -1;

        int previous_time = time_to_minutes(visitor->time_of_entry);
        int current_time = time_to_minutes(event.time);
        int time_spent = current_time - previous_time;
        table->profit += (time_spent / 60 + (time_spent % 60 > 0 ? 1 : 0)) * hour_rate;
        table->occupied_time += time_spent;

        seat_next_client_in_queue(table, event.time);
    }
    remove_client(visitor);
    free(client_name);
}

void end_of_day()
{
    for (int i = 0; i < num_clients; i++)
    {
        if (clients[i].table != -1)
        {
            Event leave_event;
            strcpy(leave_event.time, end_time);
            leave_event.id = 11;
            leave_event.data = malloc(strlen(clients[i].name) + 1);
            snprintf(leave_event.data, strlen(clients[i].name) + 1, "%s", clients[i].name);
            printf("\n%s %d %s\n", leave_event.time, leave_event.id, clients[i].name);
            client_leaves(leave_event);
            free(leave_event.data);
        }
    }
}
