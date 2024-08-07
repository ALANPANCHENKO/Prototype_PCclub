#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TIME_LENGTH 6
#define MAX_CLIENTS 100
#define MAX_EVENTS 1000
#define MAX_NAME_LENGTH 32

typedef struct Clients {
    char name[MAX_NAME_LENGTH];
    int table;
    char time_of_entry[6];
} Client;

typedef struct Tables {
    int number;
    int profit;
    int occupied_time;
    char *client;
} Table;

typedef struct Events {
    char time[MAX_TIME_LENGTH];
    int id;
    char data[256];
} Event;

typedef struct TimeRecords {
    char name[MAX_NAME_LENGTH];
    int total_time;
} TimeRecord;

Client clients[MAX_CLIENTS];
Table *tables;
TimeRecord time_records[MAX_CLIENTS];
Client *queue[MAX_CLIENTS];
Event events[MAX_EVENTS];
int num_tables = 0;
char start_time[MAX_TIME_LENGTH], end_time[MAX_TIME_LENGTH];
int hour_rate;
int num_clients;
int queue_size;
int num_events = 0;


int time_to_minutes(const char *time_str) {
    int hours, minutes;
    sscanf(time_str, "%d:%d", &hours, &minutes);
    return hours * 60 + minutes;
}

void minutes_to_time(int minutes, char *time_str) {
    int hours = minutes / 60;
    minutes = minutes % 60;
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

// int find_client_index(const char *name) {
//     for (int i = 0; i < num_clients; i++) {
//         if (strcmp(clients[i].name, name) == 0) {
//             return i;
//         }
//     }
//     return -1;
// }

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

// void print_tables_status() {
//     printf("Tables status:\n");
//     for (int i = 0; i < num_tables; i++) {
//         printf("Table %d: Client %s, Profit %d, Occupied Time %d\n", tables[i].number, tables[i].client ? tables[i].client : "None", tables[i].profit, tables[i].occupied_time);
//     }
// }

void print_table_summary() {
    char time_str[6];
    for (int i = 0; i < num_tables; i++) {
        minutes_to_time(tables[i].occupied_time, time_str);
        printf("%d %d %s\n", tables[i].number, tables[i].profit, time_str);
    }
}


// void print_clients_status() {
//     printf("Clients in club:\n");
//     for (int i = 0; i < num_clients; i++) {
//         printf("Client %s: Table %d, Entry Time %s\n", clients[i].name, clients[i].table, clients[i].time_of_entry);
//     }
// }

int compare_clients(const void *a, const void *b) {
    return strcmp(((Client *)a)->name, ((Client *)b)->name);
}

void client_arrival(Event event) {
    char client_name[MAX_NAME_LENGTH];
    sscanf(event.data, "%s", client_name);
    Client *visitor = find_client(client_name);
    if (visitor == NULL) {
        if (time_to_minutes(event.time) < time_to_minutes(start_time) || time_to_minutes(event.time) > time_to_minutes(end_time)) {
            printf("%02d:%02d 13 NotOpenYet\n", time_to_minutes(event.time) / 60, time_to_minutes(event.time) % 60);
            return;
        }
        strcpy(clients[num_clients].name, client_name);
        strcpy(clients[num_clients].time_of_entry, event.time);
        clients[num_clients].table = -1;
        visitor = &clients[num_clients];
        num_clients++;
    }
    // if (time_to_minutes(event.time) < time_to_minutes(start_time) || time_to_minutes(event.time) > time_to_minutes(end_time)) {
    //     printf("%02d:%02d 13 NotOpenYet\n", time_to_minutes(event.time) / 60, time_to_minutes(event.time) % 60);
    //     return;
    // }
    // print_tables_status();
    // print_clients_status();
}

void client_wait(Event event) {
    char client_name[MAX_NAME_LENGTH];
    sscanf(event.data, "%s", client_name);

    Client *visitor = find_client(client_name);
    int free_tables = 0;
    for (int i = 0; i < num_tables; i++) {
        if (!tables[i].client) {
            free_tables++;
        }
    }

    if (free_tables > 0) {
        printf("%s 13 ICanWaitNoLonger!\n", event.time);
        return;
    }

    if (queue_size >= num_tables) {
        printf("%s 11 %s\n", event.time, client_name);
    } else {
        queue[queue_size++] = visitor;
    }

    for (int i = 0; i < queue_size; i++) {
        printf("%d, %s\n", i + 1, queue[i]->name);
    }
    printf("%s 13 %s\n", event.time, client_name);
}



void client_seat(Event event) {
    char client_name[MAX_NAME_LENGTH];
    int table_number;
    sscanf(event.data, "%s %d", client_name, &table_number);

    Client *visitor = find_client(client_name);
    if (visitor == NULL) {
        printf("%s 13 ClientUnknown\n", event.time);
        return;
    }

    if (table_number < 1 || table_number > num_tables) {
        printf("%s 13 PlaceIsBusy\n", event.time);
        return;
    }
    Table *table = &tables[table_number - 1];
    if (table->client && strcmp(table->client, client_name) != 0) {
        printf("%s 13 PlaceIsBusy\n", event.time);
        return;
    }

    if (visitor->table != -1) {
        Table *old_table = &tables[visitor->table - 1];
        old_table->client = NULL;
    }

    if (table->client == NULL) {
        table->client = visitor->name;
    }
    strcpy(visitor->time_of_entry, event.time);
    visitor->table = table_number;

    for (int i = 0; i < queue_size; i++) {
        if (strcmp(queue[i]->name, client_name) == 0) {
            memmove(queue + i, queue + i + 1, (queue_size - i - 1) * sizeof(Client *));
            queue_size--;
            break;
        }
    }
}



void client_left(Event event) {
    char client_name[MAX_NAME_LENGTH];
    sscanf(event.data, "%s", client_name);

    Client *visitor = find_client(client_name);
    if (!visitor) {
        printf("%s 13 ClientUnknown\n", event.time);
        return;
    }

    int table_number = visitor->table;
    if (table_number != -1) {
        Table *table = &tables[table_number - 1];
        table->client = NULL;
        visitor->table = -1;

        int previous_time = time_to_minutes(visitor->time_of_entry);
        int current_time = time_to_minutes(event.time);
        int time_spent = current_time - previous_time;
        char time_str[6];
        minutes_to_time(time_spent, time_str);
        table->profit += (time_spent / 60 + (time_spent % 60 > 0 ? 1 : 0)) * hour_rate;
        table->occupied_time += time_spent;

        if (queue_size > 0) {
            Client *next_client = queue[0];
            memmove(queue, queue + 1, (--queue_size) * sizeof(Client *));

            Event seat_event;
            strcpy(seat_event.time, event.time);
            seat_event.id = 2;
            snprintf(seat_event.data, sizeof(seat_event.data), "%s %d", next_client->name, table_number);
            client_seat(seat_event);
        }
    }
    remove_client(visitor);
}

void handle_end_of_day() {
    printf("End of the day: %s\n", end_time);
    Client remaining_clients[MAX_CLIENTS];
    int remaining_count = 0;

    for (int i = 0; i < num_clients; i++) {
        if (clients[i].table != -1) {
            remaining_clients[remaining_count++] = clients[i];
        }
    }

    qsort(remaining_clients, remaining_count, sizeof(Client), compare_clients);

    for (int i = 0; i < remaining_count; i++) {
        Event leave_event;
        strcpy(leave_event.time, end_time);
        leave_event.id = 4;
        snprintf(leave_event.data, sizeof(leave_event.data), "%s", remaining_clients[i].name);
        client_left(leave_event);
    }
}

void process_events() {
    printf("Processing events\n");
    for (int i = 0; i < num_events; i++) {
        Event event = events[i];
        printf("%s %d %s\n", event.time, event.id, event.data);

        switch (event.id) {
            case 1:
                client_arrival(event);
                break;
            case 2:
                client_seat(event);
                break;
            case 3:
                client_wait(event);
                break;
            case 4:
                client_left(event);
                break;
            default:
                break;
        }
    }
    handle_end_of_day();
    print_table_summary();
}


int main(int argc, char *argv[]) {
    printf("Starting program\n");
    if (argc < 2) {
        printf("Error input\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    printf("File opened successfully\n");

    fscanf(file, "%d", &num_tables);
    fscanf(file, "%s %s", start_time, end_time);
    fscanf(file, "%d", &hour_rate);

    printf("Tables: %d, Start: %s, End: %s, Rate: %d\n", num_tables, start_time, end_time, hour_rate);
    tables = malloc(num_tables * sizeof(Table));
    for (int i = 0; i < num_tables; i++) {
        tables[i] = (Table){i + 1, 0, 0, NULL};
    }
    num_events = 0;
    while (fscanf(file, "%5s %d %[^\n]s", events[num_events].time, &events[num_events].id, events[num_events].data) != EOF) {
        num_events++;
    }

    for (int i = 0; i < num_events; i++) {
        printf("Event %d: Time: %5s, ID: %d, Data: %s\n", i + 1, events[i].time, events[i].id, events[i].data);
    }
    fclose(file);

    process_events();

    free(tables);
    return 0;
}
