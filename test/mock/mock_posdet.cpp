#include "mock_posdet.hpp"

/*
* A single, global instance of our mock object.
*/
static MockPosDet* g_mock_posdet_instance = nullptr;

/*
* A function for the tests to set the global mock instance.
*/
void set_g_mock_instance(MockPosDet* mock) {
    g_mock_posdet_instance = mock;
}

/*
* Check if a mock instance exists and, if so, forward the call to it.
*/
extern "C" {
    bool PosDet_is_elevator_position_ok(void) {
        if (g_mock_posdet_instance) {
            return g_mock_posdet_instance->PosDet_is_elevator_position_ok();
        }
        /* Default behavior if no mock is set. */
        return true;
    }

    bool PosDet_is_door_position_ok(void) {
        if (g_mock_posdet_instance) {
            return g_mock_posdet_instance->PosDet_is_door_position_ok();
        }
        /* Default behavior if no mock is set. */
        return true;
    }
}
