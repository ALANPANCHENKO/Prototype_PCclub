#include "type.h"
#include "client.h"
#include "second_utility.h"

void process_events()
{
    printf("%s\n", start_time);
    for (int i = 0; i < num_events; i++)
    {
        Event event = events[i];

        if (!is_valid_time_format(event.time))
        {
            printf("Invalid time format in event: %s %d %s\n", event.time, event.id, event.data);
            exit(1);
        }

        printf("%s %d %s", event.time, event.id, event.data);
        switch (event.id)
        {
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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Error input\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    char buffer[256];

    if (fgets(buffer, sizeof(buffer), file) == NULL || sscanf(buffer, "%d", &num_tables) != 1)
    {
        printf("Error: First line is empty.\n");
        fclose(file);
        return 1;
    }
    if (!is_valid_format(&num_tables))
    {
        printf("Invalid table format %d\n", num_tables);
        fclose(file);
        return 1;
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL || sscanf(buffer, "%s %s", start_time, end_time) != 2)
    {
        printf("Error: Second line is empty.\n");
        fclose(file);
        return 1;
    }
    if (!is_valid_time_format(start_time) || !is_valid_time_format(end_time))
    {
        printf("Invalid time format in start_time or end_time\n");
        fclose(file);
        return 1;
    }

    if (time_to_minutes(start_time) >= time_to_minutes(end_time))
    {
        printf("Error: start_time is later than end_time.\n");
        fclose(file);
        return 1;
    }

    if (fgets(buffer, sizeof(buffer), file) == NULL || sscanf(buffer, "%d", &hour_rate) != 1)
    {
        printf("Error: Third line is empty.\n");
        fclose(file);
        return 1;
    }
    if (!is_valid_format(&hour_rate))
    {
        printf("Invalid cost format %d\n", hour_rate);
        fclose(file);
        return 1;
    }

    tables = malloc(num_tables * sizeof(Table));
    for (int i = 0; i < num_tables; i++)
    {
        tables[i] = (Table){i + 1, 0, 0, NULL};
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        num_events++;
        events = realloc(events, num_events * sizeof(Event));

        sscanf(buffer, "%5s %d", events[num_events - 1].time, &events[num_events - 1].id);
        events[num_events - 1].data = malloc(strlen(buffer + 7) + 1);
        strcpy(events[num_events - 1].data, buffer + 7);
    }

    fclose(file);

    process_events();

    for (int i = 0; i < num_events; i++)
    {
        free(events[i].data);
    }
    free(events);
    free(tables);
    free(clients);
    free(queue);

    return 0;
}
