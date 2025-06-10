#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks if the elevator's current position is valid for stopping.
 *
 * In a real system, this would check if a level detector switch is active.
 * This stub version always returns true.
 *
 * @return True if the position is okay, false otherwise.
 */
bool PosDet_is_elevator_position_ok(void);

/**
 * @brief Checks if the door's current position is valid.
 *
 * In a real system, this would check the door's end-position switches.
 * This stub version always returns true.
 *
 * @return True if the position is okay, false otherwise.
 */
bool PosDet_is_door_position_ok(void);

#ifdef __cplusplus
}
#endif
