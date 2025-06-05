#pragma once

/** Condition selector module
 * This component responsible for selecting one of the inputs or a fixed zero to the output of it.
 * Additionally it is possible invert the selected value before returning.
 * +-------+-------------------------------------+
 * | Index | Selected value                      |
 * +-------+-------------------------------------+
 * |   0   | call below or same or above pending | 
 * |   1   | call below pending                  | 
 * |   2   | call same pending                   | 
 * |   3   | call above pending                  | 
 * |   4   | door closed                         | 
 * |   5   | door open                           | 
 * |   6   | reserved                            | 
 * |   7   | fixed 0 (false)                     | 
 * +-------+-------------------------------------+
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONDSEL_API
#define CONDSEL_API extern
#endif

#include <stdint.h>
#include <stdbool.h>

/** Input values of the condition selector. */
typedef struct {
	bool call_pending_below;  /* There is an active call below the elevator current level */
	bool call_pending_same;   /* There is an active call on the same level of the elevator */
	bool call_pending_above;  /* There is an active call above the elevator current level */
	bool door_closed;         /* Door is closed and locked */
	bool door_open;           /* Door is fully opened */
} CondSel_In;

/** Calculates the result of the condition selector based on the parameters.
 * @param[in] invert  Return value is inverted.
 * @param[in] index   Index of the value to select (@see documentation for details).
 * @param[in] values  External input values to select.
 * @return Resturns with the selected value or the negated value of it.
 */  
CONDSEL_API bool CondSel_calc(const bool invert, const uint8_t index, const CondSel_In values);

#ifdef __cplusplus
}
#endif
