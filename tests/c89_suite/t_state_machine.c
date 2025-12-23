/*
 * Test: State machine pattern
 * Tests: enum states, function pointer dispatch, state transitions
 */
#include <stdio.h>

/* State enumeration */
enum state {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
    STATE_ERROR,
    STATE_COUNT
};

/* Event enumeration */
enum event {
    EVENT_START,
    EVENT_PAUSE,
    EVENT_RESUME,
    EVENT_STOP,
    EVENT_RESET,
    EVENT_ERROR,
    EVENT_COUNT
};

/* State machine context */
struct state_machine {
    enum state current;
    int counter;
    int error_code;
};

/* State handler function type */
typedef enum state (*StateHandler)(struct state_machine *sm, enum event evt);

/* State handlers */
enum state handle_idle(struct state_machine *sm, enum event evt) {
    switch (evt) {
    case EVENT_START:
        sm->counter = 0;
        return STATE_RUNNING;
    case EVENT_ERROR:
        sm->error_code = 1;
        return STATE_ERROR;
    default:
        return STATE_IDLE;
    }
}

enum state handle_running(struct state_machine *sm, enum event evt) {
    switch (evt) {
    case EVENT_PAUSE:
        return STATE_PAUSED;
    case EVENT_STOP:
        return STATE_STOPPED;
    case EVENT_ERROR:
        sm->error_code = 2;
        return STATE_ERROR;
    default:
        sm->counter++;
        return STATE_RUNNING;
    }
}

enum state handle_paused(struct state_machine *sm, enum event evt) {
    switch (evt) {
    case EVENT_RESUME:
        return STATE_RUNNING;
    case EVENT_STOP:
        return STATE_STOPPED;
    case EVENT_ERROR:
        sm->error_code = 3;
        return STATE_ERROR;
    default:
        return STATE_PAUSED;
    }
}

enum state handle_stopped(struct state_machine *sm, enum event evt) {
    switch (evt) {
    case EVENT_RESET:
        sm->counter = 0;
        sm->error_code = 0;
        return STATE_IDLE;
    default:
        return STATE_STOPPED;
    }
}

enum state handle_error(struct state_machine *sm, enum event evt) {
    switch (evt) {
    case EVENT_RESET:
        sm->counter = 0;
        sm->error_code = 0;
        return STATE_IDLE;
    default:
        return STATE_ERROR;
    }
}

/* State handler table */
StateHandler state_handlers[STATE_COUNT];

/* Initialize handler table */
void init_handlers(void) {
    state_handlers[STATE_IDLE] = handle_idle;
    state_handlers[STATE_RUNNING] = handle_running;
    state_handlers[STATE_PAUSED] = handle_paused;
    state_handlers[STATE_STOPPED] = handle_stopped;
    state_handlers[STATE_ERROR] = handle_error;
}

/* Process event */
void process_event(struct state_machine *sm, enum event evt) {
    StateHandler handler;
    handler = state_handlers[sm->current];
    sm->current = handler(sm, evt);
}

/* Get state name */
char *state_name(enum state s) {
    switch (s) {
    case STATE_IDLE: return "IDLE";
    case STATE_RUNNING: return "RUNNING";
    case STATE_PAUSED: return "PAUSED";
    case STATE_STOPPED: return "STOPPED";
    case STATE_ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}

int main(void) {
    struct state_machine sm;
    int passed;
    int i;

    passed = 1;

    /* Initialize */
    init_handlers();
    sm.current = STATE_IDLE;
    sm.counter = 0;
    sm.error_code = 0;

    /* Test initial state */
    if (sm.current != STATE_IDLE) {
        printf("FAIL: initial state = %s (expected IDLE)\n", 
               state_name(sm.current));
        passed = 0;
    }

    /* IDLE -> START -> RUNNING */
    process_event(&sm, EVENT_START);
    if (sm.current != STATE_RUNNING) {
        printf("FAIL: after START: state = %s (expected RUNNING)\n",
               state_name(sm.current));
        passed = 0;
    }

    /* Run a few cycles */
    for (i = 0; i < 5; i++) {
        process_event(&sm, EVENT_COUNT); /* invalid event, stays running */
    }
    if (sm.counter != 5) {
        printf("FAIL: counter = %d (expected 5)\n", sm.counter);
        passed = 0;
    }

    /* RUNNING -> PAUSE -> PAUSED */
    process_event(&sm, EVENT_PAUSE);
    if (sm.current != STATE_PAUSED) {
        printf("FAIL: after PAUSE: state = %s (expected PAUSED)\n",
               state_name(sm.current));
        passed = 0;
    }

    /* Counter should not change while paused */
    process_event(&sm, EVENT_COUNT);
    if (sm.counter != 5) {
        printf("FAIL: counter changed while paused: %d\n", sm.counter);
        passed = 0;
    }

    /* PAUSED -> RESUME -> RUNNING */
    process_event(&sm, EVENT_RESUME);
    if (sm.current != STATE_RUNNING) {
        printf("FAIL: after RESUME: state = %s (expected RUNNING)\n",
               state_name(sm.current));
        passed = 0;
    }

    /* RUNNING -> STOP -> STOPPED */
    process_event(&sm, EVENT_STOP);
    if (sm.current != STATE_STOPPED) {
        printf("FAIL: after STOP: state = %s (expected STOPPED)\n",
               state_name(sm.current));
        passed = 0;
    }

    /* STOPPED -> RESET -> IDLE */
    process_event(&sm, EVENT_RESET);
    if (sm.current != STATE_IDLE || sm.counter != 0) {
        printf("FAIL: after RESET: state=%s, counter=%d\n",
               state_name(sm.current), sm.counter);
        passed = 0;
    }

    /* Test error handling */
    process_event(&sm, EVENT_START);
    process_event(&sm, EVENT_ERROR);
    if (sm.current != STATE_ERROR || sm.error_code != 2) {
        printf("FAIL: after ERROR: state=%s, error_code=%d\n",
               state_name(sm.current), sm.error_code);
        passed = 0;
    }

    /* ERROR -> RESET -> IDLE */
    process_event(&sm, EVENT_RESET);
    if (sm.current != STATE_IDLE || sm.error_code != 0) {
        printf("FAIL: error reset: state=%s, error_code=%d\n",
               state_name(sm.current), sm.error_code);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_state_machine\n");
    }
    return passed ? 0 : 1;
}
