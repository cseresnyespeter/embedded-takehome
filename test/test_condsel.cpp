#include <gtest/gtest.h>
#include "mock_posdet.hpp"

extern "C" {
#include "condsel.h"
}

using ::testing::Return;
using ::testing::NiceMock;


class CondSelTest : public ::testing::Test {
public:
    /* The test fixture now owns the mock object. */
    NiceMock<MockPosDet> mock_posdet;
    CondSel_In inputs {false, false, false, false, false};

protected:
    /* Before each test, install the mock and set its default behavior. */
    void SetUp() override {
        set_g_mock_instance(&mock_posdet);
    }

    void TearDown() override {
        /* Uninstall the mock after each test to ensure test isolation. */
        set_g_mock_instance(nullptr);
    }
};

TEST_F(CondSelTest, AnyCallPending_ElevatorPositionOk) {
    inputs = {false, false, false, false, false};

    ON_CALL(mock_posdet, PosDet_is_elevator_position_ok()).WillByDefault(Return(true));
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(0);

    /* Test Case 0: Any call pending. */
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 0, inputs), false);

    inputs.call_pending_below = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 0, inputs), true);

    inputs.call_pending_below = false;
    inputs.call_pending_same = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 0, inputs), true);

    inputs.call_pending_same = false;
    inputs.call_pending_above = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 0, inputs), true);
}

TEST_F(CondSelTest, AnyCallPending_ElevatorPositionNotOk) {
    inputs = {false, false, false, false, false};

    ON_CALL(mock_posdet, PosDet_is_elevator_position_ok()).WillByDefault(Return(false));
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(0);

    /* Test Case 0: Any call pending. */
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 0, inputs), false);

    inputs.call_pending_below = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 0, inputs), false);

    inputs.call_pending_below = false;
    inputs.call_pending_same = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 0, inputs), false);

    inputs.call_pending_same = false;
    inputs.call_pending_above = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 0, inputs), false);
}

TEST_F(CondSelTest, IndividualCallConditions_ElevatorPositionOk) {
    inputs = {false, false, false, false, false};

    ON_CALL(mock_posdet, PosDet_is_elevator_position_ok()).WillByDefault(Return(true));
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(0);

    /* Test Case 1: Call below. */
    inputs.call_pending_below = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 1, inputs),true);

    /* Test Case 2: Call same. */
    inputs.call_pending_same = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 2, inputs),true);

    /* Test Case 3: Call above. */
    inputs.call_pending_above = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 3, inputs),true);
}

TEST_F(CondSelTest, IndividualCallConditions_ElevatorPositionNotOk) {
    inputs = {false, false, false, false, false};

    ON_CALL(mock_posdet, PosDet_is_elevator_position_ok()).WillByDefault(Return(false));
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(0);

    /* Test Case 1: Call below. */
    inputs.call_pending_below = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 1, inputs),false);

    /* Test Case 2: Call same. */
    inputs.call_pending_same = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 2, inputs),false);

    /* Test Case 3: Call above. */
    inputs.call_pending_above = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 3, inputs),false);
}

TEST_F(CondSelTest, DoorConditions_DoorpositionOk) {
    inputs = {false, false, false, false, false};

    ON_CALL(mock_posdet, PosDet_is_door_position_ok()).WillByDefault(Return(true));
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(0);

    /* Test Case 4: Door closed. */
    inputs.door_closed = true;
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 4, inputs),true);

    /* Test Case 5: Door open. */
    inputs.door_open = true;
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 5, inputs),true);
}

TEST_F(CondSelTest, DoorConditions_DoorpositionNotOk) {
    inputs = {false, false, false, false, false};

    ON_CALL(mock_posdet, PosDet_is_door_position_ok()).WillByDefault(Return(false));
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(0);

    /* Test Case 4: Door closed. */
    inputs.door_closed = true;
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 4, inputs),false);

    /* Test Case 5: Door open. */
    inputs.door_open = true;
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(false, 5, inputs),false);
}

TEST_F(CondSelTest, AlwaysFalse) {
    inputs = {false, false, false, false, false};

    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(0);
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(0);

    /* Test Case 6: Reserved. */
    EXPECT_EQ(CondSel_calc(false, 6, inputs),false);

    /* Test Case 7: Const 0. */
    EXPECT_EQ(CondSel_calc(false, 7, inputs),false);
}

TEST_F(CondSelTest, Inversion) {
    inputs = {false, false, false, false, false};

    ON_CALL(mock_posdet, PosDet_is_elevator_position_ok()).WillByDefault(Return(true));
    ON_CALL(mock_posdet, PosDet_is_door_position_ok()).WillByDefault(Return(true));

    /* Invert a true condition. */
    inputs.call_pending_below = true;
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(true, 1, inputs),false);

    /* Invert a false condition. */
    inputs.door_closed = false;
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(1);
    EXPECT_EQ(CondSel_calc(true, 4, inputs),true);

    /* Invert the always false condition. */
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(0);
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(0);
    EXPECT_EQ(CondSel_calc(true, 7, inputs),true);
}

TEST_F(CondSelTest, InvalidIndex) {
    inputs = {false, false, false, false, false};

    /* Use an index that is not defined. */
    EXPECT_CALL(mock_posdet, PosDet_is_door_position_ok()).Times(0);
    EXPECT_CALL(mock_posdet, PosDet_is_elevator_position_ok()).Times(0);
    EXPECT_EQ(CondSel_calc(false, 8, inputs),false);
    EXPECT_EQ(CondSel_calc(false, 255, inputs),false);
}
