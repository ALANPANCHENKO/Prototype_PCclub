#include "type.h"
#include "client.h"
#include "second_utility.h"

void process_events() {
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
                client_leaves(event);
                break;
            default:
                break;
        }
    }
    end_of_day();
    print_table_summary();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error input\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fscanf(file, "%d", &num_tables);
    fscanf(file, "%s %s", start_time, end_time);
    fscanf(file, "%d", &hour_rate);

    tables = malloc(num_tables * sizeof(Table));
    for (int i = 0; i < num_tables; i++) {
        tables[i] = (Table){i + 1, 0, 0, NULL};
    }

    while (fscanf(file, "%5s %d %[^\n]s", events[num_events].time, &events[num_events].id, events[num_events].data) != EOF) {
        num_events++;
    }

    fclose(file);

    process_events();
    free(tables);
    return 0;
}
