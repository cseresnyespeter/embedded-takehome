#include "condsel.h"
#include "posdet.h"

/**
 * @brief Calculates the result of the condition selectors.
 *
 * This function acts as a multiplexer for various system conditions. It selects a
 * condition based on an index and can optionally invert the result.
 *
 * @param[in] invert  If true, the final result is inverted.
 * @param[in] index   Index of the condition to select.
 * @param[in] values  Struct containing the current state of all conditions.
 * @return The boolean result of the selected condition.
 */
CONDSEL_API bool CondSel_calc(const bool invert, const uint8_t index, const CondSel_In values) {
    bool result = false;

    switch (index) {
        case 0U:
            /* Any call is pending. */
            if (PosDet_is_elevator_position_ok())
            {
                result = values.call_pending_below || values.call_pending_same || values.call_pending_above;
            }
            break;
        case 1U:
            /* A call is pending below the current floor. */
            if (PosDet_is_elevator_position_ok())
            {
                result = values.call_pending_below;
            }
            break;
        case 2U:
            /* A call is pending on the current floor. */
            if (PosDet_is_elevator_position_ok())
            {
                result = values.call_pending_same;
            }

            break;
        case 3U:
            /* A call is pending above the current floor. */
            if (PosDet_is_elevator_position_ok())
            {
                result = values.call_pending_above;
            }
            break;
        case 4U:
            /* The door is fully closed. */
            if (PosDet_is_door_position_ok())
            {
                result = values.door_closed;
            }
            break;
        case 5U:
            /* The door is fully open. */
            if (PosDet_is_door_position_ok())
            {
                result = values.door_open;
            }
            break;
        case 6U:
        case 7U:
            /* Always inactive. */
            result = false;
            break;
        default:
            result = false;
            break;
    }

    /* Apply inversion if needed. */
    if (invert)
    {
        result = !result;
    }

    return result;
}
