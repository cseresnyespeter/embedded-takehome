#pragma once

/**#################################################################################################
 * Sequential network module
 * #################################################################################################
 * This component responsible to interpret a program memory and evaluate it. The ProgMem needs to be
 * defined here. A line (instruction) of the memory consists of the following bit-fields:
 * Additionally it is possible invert the selected value before returning.
 * +--------+--------------------------------------------------------------------------------+
 * | Bits   | Description                                                                    |
 * +--------+--------------------------------------------------------------------------------+
 * |  7..0  | jump address - needs to be loaded to PC if condition is true                   | 
 * |    8   | request to move the elevator upwards                                           | 
 * |    9   | request to move the elevator downwards                                         | 
 * |   10   | target door state (0: closed, 1: open)                                         | 
 * |   11   | request to clear the pending bit from active call memory for the current floor | 
 * | 13..12 | condition select index                                                         | 
 * |   14   | activates the inversion of the value of the selected condition value           | 
 * +--------+--------------------------------------------------------------------------------+
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SEQNET_API
#define SEQNET_API extern
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	bool cond_inv;        /* Condition value inversion */
	uint8_t cond_sel;     /* Condition value selection */
	bool req_reset;       /* Reset call state for the actual level */
	bool req_door_state;  /* Request to open the door (request to close if false) */
	bool req_move_down;   /* Request to move the elevator to a lower level */
	bool req_move_up;     /* Request to move the elevator to a higher level */
	uint8_t jump_addr;    /* Address to jump if condition result is active */
} SeqNet_Out;

/** Initializes the sequential network internal state.
  * Note: needs to be called only once at startup
  */
SEQNET_API void SeqNet_init(void);

/** Steps the sequential network to the next state.
  * @param[in] condition_active  True, if the selected condition value is active (or inactive if inversion is activate)
  * @return Returns with the new instruction values (@see SeqNet_Out).
  */
SEQNET_API SeqNet_Out SeqNet_loop(const bool condition_active); 

#ifdef __cplusplus
}
#endif
