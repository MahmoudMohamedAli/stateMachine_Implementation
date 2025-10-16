#include <stdio.h>
#include <stdbool.h>

// --- 1. Define States (Current State) ---
typedef enum
{
    STATE_IDLE,         // Waiting for a command or data
    STATE_READ_SENSOR,  // Actively reading a sensor or peripheral
    STATE_PROCESS_DATA, // Crunching the numbers
    STATE_FAULT,        // Unrecoverable error state
    NUM_STATES          // Used for array sizing/validation
} State_t;

// --- 2. Define Events (Input/Trigger - your 'rc') ---
typedef enum
{
    EVENT_COMMAND_RX,    // Command received
    EVENT_READ_COMPLETE, // Sensor reading finished
    EVENT_CALC_SUCCESS,  // Data processing succeeded
    EVENT_CALC_FAILURE,  // Data processing failed (e.g., overflow)
    EVENT_TIMEOUT,       // A timer expired
    NUM_EVENTS           // Used for array sizing/validation
} Event_t;

// --- 3. Define the Transition Structure ---
// A single row in our logic table: (Current State, Event) -> Next State
typedef struct
{
    State_t current_state;
    Event_t event;
    State_t next_state;
} Transition_t;

// --- 4. The Core Transition Table ---
// This is the complete logic of the state machine.
static const Transition_t transition_table[] = {
    // Current State       | Event               | Next State
    // --------------------+---------------------+-------------------
    {STATE_IDLE, EVENT_COMMAND_RX, STATE_READ_SENSOR},
    {STATE_IDLE, EVENT_TIMEOUT, STATE_IDLE}, // Stay in IDLE on timeout

    {STATE_READ_SENSOR, EVENT_READ_COMPLETE, STATE_PROCESS_DATA},
    {STATE_READ_SENSOR, EVENT_TIMEOUT, STATE_FAULT}, // A timeout here is a critical failure

    {STATE_PROCESS_DATA, EVENT_CALC_SUCCESS, STATE_IDLE},  // Successful cycle complete
    {STATE_PROCESS_DATA, EVENT_CALC_FAILURE, STATE_FAULT}, // Critical calculation failure
};

// Calculate the number of transitions in the table
static const size_t TRANSITION_COUNT = sizeof(transition_table) / sizeof(Transition_t);

/**
 * @brief Looks up the next state based on the current state and the received event.
 * * @param current_state The current state of the machine.
 * @param event The input event (your 'rc' parameter).
 * @return State_t The next state to transition to, or the current state if no valid transition is found.
 */
State_t lookup_transitions(State_t current_state, Event_t event)
{
    for (size_t i = 0; i < TRANSITION_COUNT; i++)
    {
        const Transition_t *t = &transition_table[i];

        // Match the current state AND the event
        if (t->current_state == current_state && t->event == event)
        {
            return t->next_state;
        }
    }

    // --- Safety Feature: Handle Invalid Transition ---
    // If we reach here, an event was received that is illegal in the current state.
    // In firmware, we often log this as an error but keep the current state
    // to prevent jumping to an unknown or uninitialized state.
    printf("WARNING: Illegal transition detected: State %d received Event %d. Staying in current state.\n",
           current_state, event);

    return current_state;
}

// Utility function to convert enum to string for logging
const char *state_to_string(State_t state)
{
    switch (state)
    {
    case STATE_IDLE:
        return "IDLE";
    case STATE_READ_SENSOR:
        return "READ_SENSOR";
    case STATE_PROCESS_DATA:
        return "PROCESS_DATA";
    case STATE_FAULT:
        return "FAULT";
    default:
        return "UNKNOWN";
    }
}

// --- Demonstration / Main Loop ---
int main()
{
    State_t current_state = STATE_IDLE;
    State_t next_state;
    Event_t test_events[] = {
        EVENT_COMMAND_RX,    // 1. Valid: IDLE -> READ_SENSOR
        EVENT_READ_COMPLETE, // 2. Valid: READ_SENSOR -> PROCESS_DATA
        EVENT_CALC_FAILURE,  // 3. Valid: PROCESS_DATA -> FAULT
        EVENT_CALC_SUCCESS,  // 4. Invalid in FAULT state
        EVENT_TIMEOUT        // 5. Invalid in FAULT state
    };
    size_t num_test_events = sizeof(test_events) / sizeof(Event_t);

    printf("--- State Machine Demo ---\n");
    printf("Initial State: %s\n\n", state_to_string(current_state));

    for (size_t i = 0; i < num_test_events; i++)
    {
        Event_t rc = test_events[i];

        printf("--- Step %zu ---\n", i + 1);
        printf("Event received (rc): %d\n", rc);
        printf("Before: %s\n", state_to_string(current_state));

        // THE IMPLEMENTATION OF YOUR QUERY:
        next_state = lookup_transitions(current_state, rc);

        // Transition the state
        if (next_state != current_state)
        {
            printf("Transition: %s -> %s\n", state_to_string(current_state), state_to_string(next_state));
            current_state = next_state;
        }
        printf("After: %s\n\n", state_to_string(current_state));

        // Stop processing if we hit a terminal fault state
        if (current_state == STATE_FAULT)
        {
            printf("SYSTEM HALTED due to FAULT state.\n");
            break;
        }
    }

    return 0;
}
