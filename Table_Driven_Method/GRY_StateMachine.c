#include <stdio.h>
#include <unistd.h> // Required for usleep() on Linux/macOS
#include <string.h>

typedef enum State State_t;
typedef void (*pStateAction)(State_t state);
const char *convert_Enum_to_str(State_t currState);

void Red_handler(State_t state);
void Yellow_handler(State_t state);
void Green_handler(State_t state);

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
    pStateAction action;
    State_t next;

} Transition_t;

Transition_t transition_Table[] = {
    {RED, TIMEOUT, Red_handler,YELLOW},
    {YELLOW, TIMEOUT, Yellow_handler,GREEN},
    {GREEN, TIMEOUT, Green_handler,RED},
};

//handler for each state
void Red_handler(State_t state)
{
    puts("Hello Red state!");
}
void Yellow_handler(State_t state)
{
    puts("Hello Yellow state!");
}

void Green_handler(State_t state)
{
    puts("Hello Green state!");
}



const int Transition_Count = sizeof(transition_Table) / sizeof(Transition_t);
State_t lookup_Transition(State_t curr, Event_t event)
{
    Transition_t *it = &transition_Table[0];
    State_t nextState;
    for (size_t i = 0; i < Transition_Count; i++)
    {
        if (curr == it[i].curr && event == it[i].event)
        {
            it[i].action(nextState);
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