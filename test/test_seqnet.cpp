#include <gtest/gtest.h>

extern "C" {
#include "seqnet.h"
}

class SeqNetTest : public ::testing::Test {
};

TEST_F(SeqNetTest, Initialization) {
    SeqNet_init();

    SeqNet_Out out = SeqNet_loop(false);

    /* We expect to be in the IDLE state, which checks for any pending call. */
    /* and requests the door to be open. */
    EXPECT_EQ(out.cond_sel, 0);
    EXPECT_EQ(out.req_door_state,true);
    EXPECT_EQ(out.req_move_up,false);
    EXPECT_EQ(out.req_move_down,false);
}

TEST_F(SeqNetTest, IdleToCloseDoorTransition) {
    SeqNet_init();
    /* Start at initial state. */
    SeqNet_Out out = SeqNet_loop(false);

    /* Call is pending. */
    out = SeqNet_loop(true);

    /* We expect to be in the CLOSE_DOOR state, which checks checks if the door is closed. */
    /* and requests the door to close. */
    EXPECT_EQ(out.cond_sel, 4);
    EXPECT_EQ(out.req_door_state,false);
}

TEST_F(SeqNetTest, FullSequence_GroundToFloor2) {
    SeqNet_init();

    /* PC jumps to IDLE. */
    SeqNet_Out out = SeqNet_loop(false);
    EXPECT_EQ(out.cond_sel, 0);

    /* Transition to CLOSE_DOOR state. Checking if door is closed. */
    out = SeqNet_loop(true);
    EXPECT_EQ(out.cond_sel, 4);
    EXPECT_EQ(out.req_door_state,false);

    /* Door is not yet closed. */
    /* PC is incremented. */
    out = SeqNet_loop(false);
    EXPECT_EQ(out.cond_sel, 7);
    EXPECT_EQ(out.req_door_state,false);

    /* PC jumps back to CLOSE_DOOR. */
    out = SeqNet_loop(true);
    EXPECT_EQ(out.cond_sel, 4);

    /* Door is closed. */
    /* PC jumps to CHOOSE_DIRECTION state. */
    out = SeqNet_loop(true);
    /* The call is from above. */
    EXPECT_EQ(out.cond_sel, 3);

    /* PC jumps to MOVE_UP state. */
    out = SeqNet_loop(true);
    EXPECT_EQ(out.cond_sel, 2);
    EXPECT_EQ(out.req_move_up,false);

    /* PC incremented. */
    out = SeqNet_loop(false);
    EXPECT_EQ(out.cond_sel, 7);
    EXPECT_EQ(out.req_move_up,true);

    /* PC jumps to MOVE_UP state. */
    out = SeqNet_loop(true);
    EXPECT_EQ(out.cond_sel, 2);
    EXPECT_EQ(out.req_move_up,false);

    /* PC incremented. */
    out = SeqNet_loop(false);
    EXPECT_EQ(out.cond_sel, 7);
    EXPECT_EQ(out.req_move_up,true);

     /* PC jumps to MOVE_UP state. */
    out = SeqNet_loop(true);
    EXPECT_EQ(out.cond_sel, 2);
    EXPECT_EQ(out.req_move_up,false);

    /* PC jumps to  ARRIVED state. */
    out = SeqNet_loop(true);
    EXPECT_EQ(out.cond_sel, 5);
    EXPECT_EQ(out.req_door_state,true);
    EXPECT_EQ(out.req_reset,true);

    /* Test SeqNet_init. */
    SeqNet_init();
    /* PC jumps to IDLE. */
    out = SeqNet_loop(false);
    EXPECT_EQ(out.cond_sel, 0);

}
