#include <stdio.h>
#include <unistd.h> // Required for usleep() on Linux/macOS
#include <string.h>

typedef enum State State_t;
const char *convert_Enum_to_str(State_t currState);

typedef enum State
{
    RED,
    YELLOW,
    GREEN,
    MAX_NUM_STATES
    // add more states if needed
} State_t;

typedef enum Event
{
    TIMEOUT,
    MAX_NUM_EVENTS
    // add more events if needed
} Event_t;

typedef struct Transition
{
    State_t curr;
    Event_t event;
    State_t next;

} Transition_t;

Transition_t transition_Table[] = {
    {RED, TIMEOUT, YELLOW},
    {YELLOW, TIMEOUT, GREEN},
    {GREEN, TIMEOUT, RED},
};

const int Transition_Count = sizeof(transition_Table) / sizeof(Transition_t);
State_t lookup_Transition(State_t curr, Event_t event)
{
    Transition_t *it = &transition_Table[0];
    State_t nextState;
    for (size_t i = 0; i < Transition_Count; i++)
    {
        if (curr == it[i].curr && event == it[i].event)
        {
            nextState = it[i].next;
        }
    }
    // check if the event is invalid
    return nextState;
}

const char *convert_Enum_to_str(State_t currState)
{
    switch (currState)
    {
    case RED:
        return ("RED");
    case YELLOW:
        return ("YELLOW");
    case GREEN:
        return ("GREEN");
    default:
        return "UNKNOWN";
    }
}
int main()
{
    State_t cur_state = RED;
    printf("start ...\n");
    while (1)
    {
        sleep(3);
        printf("%s ->  ", convert_Enum_to_str(cur_state));
        cur_state = lookup_Transition(cur_state, TIMEOUT);
        printf("%s\n", convert_Enum_to_str(cur_state));
        fflush(stdout);
    }
}