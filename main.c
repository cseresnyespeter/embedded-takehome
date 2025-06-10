#include <stdio.h>
#include "seqnet.h"
#include "condsel.h"

#define NUM_FLOORS 6U

/* Represents the state of the doors. */
typedef enum
{
    DOOR_STATE_OPEN,
    DOOR_STATE_CLOSED
} DoorStatus;

/* Represents the state of the elevator's movement. */
typedef enum
{
    MOVEMENT_STOPPED,
    MOVEMENT_UP,
    MOVEMENT_DOWN
} MovementStatus;

/* Simulation structure. */
typedef struct
{
    uint8_t current_floor;
    DoorStatus door_status;
    MovementStatus movement_status;
    bool pending_calls[NUM_FLOORS];
} ElevatorSimulation;

/* Variable to store previous condition state. */
static bool condition_active = false;

/**
 * @brief initialises the simulator.
 */
static void init_simulation()
{
    SeqNet_init();
    condition_active = false;
}

/**
 * @brief Updates the state of the elevator based on the requests.
 * @param sim The elevator simulation state to update.
 * @param controller_out The requests from the sequential network controller.
 */
static void update_simulation(ElevatorSimulation *sim, const SeqNet_Out *controller_out)
{
    /* Handle Door Logic only when movement is stopped. */
    if (sim->movement_status == MOVEMENT_STOPPED)
    {
        sim->door_status = controller_out->req_door_state ? DOOR_STATE_OPEN : DOOR_STATE_CLOSED;
    }

    /* Handle Movement Logic only when the door is closed. */
    if (sim->door_status == DOOR_STATE_CLOSED)
    {
        if (controller_out->req_move_up && (sim->current_floor < (NUM_FLOORS - 1U)))
        {
            sim->movement_status = MOVEMENT_UP;
        }
        else if (controller_out->req_move_down && (sim->current_floor > 0U))
        {
            sim->movement_status = MOVEMENT_DOWN;
        }
        else
        {
            sim->movement_status = MOVEMENT_STOPPED;
        }
    }
    else
    {
        sim->movement_status = MOVEMENT_STOPPED;
    }

    /* Update floor position. */
    if (sim->movement_status == MOVEMENT_UP)
    {
        sim->current_floor++;
    }
    else if (sim->movement_status == MOVEMENT_DOWN)
    {
        sim->current_floor--;
    }

    /* Handle Pending Call Reset Request. */
    if (controller_out->req_reset)
    {
        sim->pending_calls[sim->current_floor] = false;
    }
}

/**
 * @brief Runs the simulation until there are no pending calls and the door is open.
 * @param sim The elevator simulation state.
 * @param max_steps The maximum number of simulation cycles to run.
 */
static void run_simulation_steps(ElevatorSimulation *sim, uint16_t max_steps)
{
    SeqNet_Out controller_outputs = {0};
    bool any_calls_pending = false;

    for (uint16_t i = 0U; i < max_steps; i++)
    {
        /* Step the controller to get new requests. */
        controller_outputs = SeqNet_loop(condition_active);

        /* Update the simulation based on the controller's requests. */
        update_simulation(sim, &controller_outputs);

        printf(" Floor=%d, Movement=%d, Door=%s, Calls=",
                     sim->current_floor,
                     sim->movement_status,
                     sim->door_status == DOOR_STATE_OPEN ? "Open  " : "Closed");

        for (uint8_t j = 0U; j < NUM_FLOORS; j++)
        {
            printf("%d", sim->pending_calls[j]);
        }
        printf("\n");

        /* Prepare the inputs for the next controller cycle. */
        CondSel_In condition_inputs = {false, false, false, false, false};
        condition_inputs.door_open = (sim->door_status == DOOR_STATE_OPEN);
        condition_inputs.door_closed = (sim->door_status == DOOR_STATE_CLOSED);

        any_calls_pending = false;
        for (uint8_t k = 0U; k < NUM_FLOORS; k++)
        {
            if (sim->pending_calls[k])
            {
                any_calls_pending = true;
                if (k > sim->current_floor)
                {
                    condition_inputs.call_pending_above = true;
                }
                else if (k < sim->current_floor)
                {
                    condition_inputs.call_pending_below = true;
                }
                else
                {
                    condition_inputs.call_pending_same = true;
                }
            }
        }

        /* Calculate the condition result that the controller will use in the next loop. */
        condition_active = CondSel_calc(controller_outputs.cond_inv, controller_outputs.cond_sel, condition_inputs);

        /* Stop if no calls are pending and the door is open. */
        if ((any_calls_pending == false) && (sim->door_status == DOOR_STATE_OPEN))
        {
            printf("\nTest finished\n");
            break;
        }
    }
}

int main(void)
{
    ElevatorSimulation sim = {
        .current_floor = 0U,
        .door_status = DOOR_STATE_OPEN,
        .movement_status = MOVEMENT_STOPPED,
        .pending_calls = {false}};

    /* TEST 1: Call from floor 0 to floor 3. */
    printf("\nTEST 1: Call to Floor 3\n");
    sim.current_floor = 0U;
    sim.door_status = DOOR_STATE_OPEN;
    sim.movement_status = MOVEMENT_STOPPED;
    sim.pending_calls[3] = true;
    init_simulation();
    run_simulation_steps(&sim, 100U);

    /* TEST 2: Multiple pending calls (floor 5, then floor 1). */
    printf("\nTEST 2: Calls to Floor 5 and 1\n");
    sim.current_floor = 0U;
    sim.door_status = DOOR_STATE_OPEN;
    sim.movement_status = MOVEMENT_STOPPED;
    sim.pending_calls[1] = true;
    sim.pending_calls[5] = true;
    init_simulation();
    run_simulation_steps(&sim, 100U);

    /* TEST 3: Multiple pending calls, starting from floor 3 (floor 5, then floor 1). */
    printf("\nTEST 3:Calls to Floor 5 and 1 from Floor 3\n");
    sim.current_floor = 3U;
    sim.door_status = DOOR_STATE_OPEN;
    sim.movement_status = MOVEMENT_STOPPED;
    sim.pending_calls[1] = true;
    sim.pending_calls[5] = true;
    init_simulation();
    run_simulation_steps(&sim, 100U);


    /* TEST 4: New call during movement. */
    printf("\nTEST 4: Call during movement\n");
    sim.current_floor = 0U;
    sim.door_status = DOOR_STATE_OPEN;
    sim.movement_status = MOVEMENT_STOPPED;
    sim.pending_calls[5] = true;
    init_simulation();
    run_simulation_steps(&sim, 10U);

    printf("\nADDING NEW CALL TO 0 MID-TRIP\n");
    sim.pending_calls[0] = true;
    run_simulation_steps(&sim, 100U);

    /* TEST 5: Call from floor 0 to floor 0. */
    printf("\nTEST 5: Call to Floor 0\n");
    sim.current_floor = 0U;
    sim.door_status = DOOR_STATE_OPEN;
    sim.movement_status = MOVEMENT_STOPPED;
    sim.pending_calls[0] = true;
    init_simulation();
    run_simulation_steps(&sim, 100U);

    printf("\nATests finished.\n");
    return 0;
}
