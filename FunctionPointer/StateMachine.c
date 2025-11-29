#include <stdio.h>
#include <stdlib.h>

// 1. Define the possible inputs (events)
typedef enum {
    EVT_COIN_INSERTED,  // Input: Payment is dropped
    EVT_PUSH_TURNSTILE, // Input: Person attempts to pass
    NUM_EVENTS
} Event;

// 2. Typedef for the state function signature:
// All state handlers must conform to this signature.
typedef void (*StateFunction)(Event);

// Global function pointer to the current state handler (the core mechanism)
StateFunction current_state;

// Forward declarations
void handle_locked(Event event);
void handle_unlocked(Event event);                                                   


// State 1: LOCKED Handler
void handle_locked(Event event) {
    printf("State: **LOCKED** | Received Event: %d\n", event);

    switch (event) {
        case EVT_COIN_INSERTED:
            printf("  -> Action: Coin accepted. Transitioning to UNLOCKED.\n");
            // TRANSITION: Change the function pointer to the UNLOCKED handler
            current_state = handle_unlocked; 
            break;

        case EVT_PUSH_TURNSTILE:
            printf("  -> Action: BLOCKED. Remains LOCKED.\n");
            // No transition
            break;
            
        default:
            break;
    }
}

// State 2: UNLOCKED Handler
void handle_unlocked(Event event) {
    printf("State: **UNLOCKED** | Received Event: %d\n", event);

    switch (event) {
        case EVT_COIN_INSERTED:
            printf("  -> Action: Coin rejected (refunded). Remains UNLOCKED.\n");
            // No transition
            break;

        case EVT_PUSH_TURNSTILE:
            printf("  -> Action: Person passes. Transitioning to LOCKED.\n");
            // TRANSITION: Change the function pointer back to the LOCKED handler
            current_state = handle_locked; 
            break;

        default:
            break;
    }
}

void run_state_machine(Event event) {
    printf("\n--- Processing Event %d ---\n", event);
    if (current_state != NULL) {
        // Execute the function currently pointed to by current_state
        current_state(event); 
    }
}

int main() {
    printf("=== Turnstile State Machine Simulation ===\n");

    // Initialize the state machine to the starting state
    current_state = handle_locked;
    printf("Initial State: LOCKED\n");

    // Scenario 1: Coin is inserted (Locked -> Unlocked)
    run_state_machine(EVT_COIN_INSERTED); 

    // Scenario 2: Person pushes (Unlocked -> Locked)
    run_state_machine(EVT_PUSH_TURNSTILE); 

    // Scenario 3: Person tries to push again (Locked -> Blocked/Stays Locked)
    run_state_machine(EVT_PUSH_TURNSTILE); 

    printf("\n=== Simulation Complete ===\n");

    return 0;
}
