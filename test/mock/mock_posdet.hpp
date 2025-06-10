#pragma once

#include <gmock/gmock.h>
#include "posdet.h"

class MockPosDet {
public:
    virtual ~MockPosDet() = default;

    MOCK_METHOD(bool, PosDet_is_elevator_position_ok, (), ());
    MOCK_METHOD(bool, PosDet_is_door_position_ok, (), ());
};

void set_g_mock_instance(MockPosDet* mock);