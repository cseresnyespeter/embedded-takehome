#include "seqnet.h"
#include <stdbool.h>
#include <stdint.h>

#define PROG_MEM_SIZE 256U

/* Bit positions of instructions. */
#define BIT_POS_INV         15U
#define BIT_POS_COND_SEL    12U
#define BIT_POS_RESET       11U
#define BIT_POS_DOOR        10U
#define BIT_POS_DOWN        9U
#define BIT_POS_UP          8U

/* Masks for decoding multi-bit fields. */
#define MASK_COND_SEL       0x7U
#define MASK_JUMP_ADDR      0xFFU

/* Pre-shifted bit masks for constructing single-bit instruction fields. */
#define FIELD_INV           (1U << BIT_POS_INV)
#define FIELD_RESET         (1U << BIT_POS_RESET)
#define FIELD_DOOR_OPEN     (1U << BIT_POS_DOOR)
#define FIELD_DOWN          (1U << BIT_POS_DOWN)
#define FIELD_UP            (1U << BIT_POS_UP)

/* Program Counter. */
static uint8_t PC;

/* Condition Selector index values. */
typedef enum {
    COND_ANY_CALL     = 0,
    COND_CALL_BELOW   = 1,
    COND_CALL_SAME    = 2,
    COND_CALL_ABOVE   = 3,
    COND_DOOR_CLOSED  = 4,
    COND_DOOR_OPEN    = 5,
    COND_RESERVED     = 6,
    COND_ALWAYS_FALSE = 7
} ConditionIndex;

/* Pre-shifted bit masks for the condition selection field. */
#define COND_SEL_ANY_CALL     ((uint16_t)COND_ANY_CALL << BIT_POS_COND_SEL)
#define COND_SEL_CALL_BELOW   ((uint16_t)COND_CALL_BELOW << BIT_POS_COND_SEL)
#define COND_SEL_CALL_SAME    ((uint16_t)COND_CALL_SAME << BIT_POS_COND_SEL)
#define COND_SEL_CALL_ABOVE   ((uint16_t)COND_CALL_ABOVE << BIT_POS_COND_SEL)
#define COND_SEL_DOOR_CLOSED  ((uint16_t)COND_DOOR_CLOSED << BIT_POS_COND_SEL)
#define COND_SEL_DOOR_OPEN    ((uint16_t)COND_DOOR_OPEN << BIT_POS_COND_SEL)
#define COND_SEL_ALWAYS_FALSE ((uint16_t)COND_ALWAYS_FALSE << BIT_POS_COND_SEL)

/* Program Counter locations, defining the states of the machine. */
typedef enum {
    STATE_INIT          = 0,  /* Length: 1. */
    STATE_IDLE          = 1,  /* Length: 3. */
    STATE_CLOSE_DOOR    = 4,  /* Length: 2. */
    STATE_CHOOSE_DIR    = 6,  /* Length: 4. */
    STATE_MOVE_UP       = 10, /* Length: 2. */
    STATE_MOVE_DOWN     = 12, /* Length: 2. */
    STATE_ARRIVED       = 14  /* Length: 2. */
} ProgramState;

/* The program memory defines the elevator's state machine logic. */
static const uint16_t ProgMem[PROG_MEM_SIZE] = {
    /*
     * STATE: INIT.
     * Purpose: Entry point on power-up. Unconditional jump to the main IDLE state.
     */
    [STATE_INIT] = FIELD_INV | COND_SEL_ALWAYS_FALSE | (uint16_t)STATE_IDLE,

    /*
     * STATE: IDLE.
     * Purpose: Wait for a call with the door open.
     */
    [STATE_IDLE]   = COND_SEL_CALL_SAME  | FIELD_RESET | FIELD_DOOR_OPEN | (uint16_t)STATE_IDLE,
    [STATE_IDLE+1] = COND_SEL_ANY_CALL   | FIELD_DOOR_OPEN | (uint16_t)STATE_CLOSE_DOOR,
    [STATE_IDLE+2] = FIELD_INV | COND_SEL_ALWAYS_FALSE | FIELD_DOOR_OPEN | (uint16_t)STATE_IDLE,

    /*
     * STATE: CLOSE_DOOR.
     * Purpose: A call is registered, so close the door.
     */
    [STATE_CLOSE_DOOR]   = COND_SEL_DOOR_CLOSED | (uint16_t)STATE_CHOOSE_DIR,
    [STATE_CLOSE_DOOR+1] = FIELD_INV | COND_SEL_ALWAYS_FALSE | (uint16_t)STATE_CLOSE_DOOR,

    /*
     * STATE: CHOOSE_DIRECTION.
     * Purpose: Door is closed. Decide which direction to move.
     */
    [STATE_CHOOSE_DIR]   = COND_SEL_CALL_ABOVE | (uint16_t)STATE_MOVE_UP,
    [STATE_CHOOSE_DIR+1] = COND_SEL_CALL_BELOW | (uint16_t)STATE_MOVE_DOWN,
    [STATE_CHOOSE_DIR+2] = COND_SEL_CALL_SAME | (uint16_t)STATE_ARRIVED,
    [STATE_CHOOSE_DIR+3] = FIELD_INV | COND_SEL_ALWAYS_FALSE | (uint16_t)STATE_IDLE,

    /*
     * STATE: MOVE_UP.
     * Purpose: Check for arrival before moving to prevent overshoot.
     */
    [STATE_MOVE_UP]   = COND_SEL_CALL_SAME | (uint16_t)STATE_ARRIVED,
    [STATE_MOVE_UP+1] = FIELD_INV | COND_SEL_ALWAYS_FALSE | FIELD_UP | (uint16_t)STATE_MOVE_UP,

    /*
     * STATE: MOVE_DOWN.
     * Purpose: Check for arrival before moving to prevent overshoot.
     */
    [STATE_MOVE_DOWN]   = COND_SEL_CALL_SAME | (uint16_t)STATE_ARRIVED,
    [STATE_MOVE_DOWN+1] = FIELD_INV | COND_SEL_ALWAYS_FALSE | FIELD_DOWN | (uint16_t)STATE_MOVE_DOWN,

    /*
     * STATE: ARRIVED.
     * Purpose: Arrived at a destination floor.
     */
    [STATE_ARRIVED]   = COND_SEL_DOOR_OPEN | FIELD_RESET | FIELD_DOOR_OPEN | (uint16_t)STATE_IDLE,
    [STATE_ARRIVED+1] = FIELD_INV | COND_SEL_ALWAYS_FALSE | FIELD_RESET | FIELD_DOOR_OPEN | (uint16_t)STATE_ARRIVED
};


/**
 * @brief Initializes the ProgramCounter.
 */
void SeqNet_init(void)
{
    PC = 0U;
}

/**
 * @brief Steps the sequential network to the next state.
 *
 * This function performs one cycle of the network. It first determines the
 * next value of the Program Counter (PC) based on the result of the previous
 * cycle's condition. Then, it fetches and decodes the instruction at the new PC
 * location and returns it.
 *
 * @param[in] condition_active  The result of the condition check from the previous cycle.
 * @return The new instruction containing requests for the system and condition
 * selection for the next cycle.
 */
SeqNet_Out SeqNet_loop(const bool condition_active)
{
    /* Read the jump address from the instruction at the CURRENT PC. */
    uint16_t current_instruction = ProgMem[PC];
    uint8_t jump_addr = (uint8_t)(current_instruction & MASK_JUMP_ADDR);

    /* Update the PC for the next cycle based on the condition result. */
    if (condition_active)
    {
        PC = jump_addr;
    }
    else
    {
        PC++;
    }

    /* Load the instruction at the new PC location. */
    uint16_t new_instruction = ProgMem[PC];

    /* Decode the new instruction into the output struct. */
    SeqNet_Out out;
    out.jump_addr = (uint8_t)(new_instruction & MASK_JUMP_ADDR);
    out.req_move_up = (bool)((new_instruction >> BIT_POS_UP) & 1U);
    out.req_move_down = (bool)((new_instruction >> BIT_POS_DOWN) & 1U);
    out.req_door_state = (bool)((new_instruction >> BIT_POS_DOOR) & 1U);
    out.req_reset = (bool)((new_instruction >> BIT_POS_RESET) & 1U);
    out.cond_sel = (uint8_t)((new_instruction >> BIT_POS_COND_SEL) & MASK_COND_SEL);
    out.cond_inv = (bool)((new_instruction >> BIT_POS_INV) & 1U);

    return out;
}
