// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#ifndef MOUSE_CODES_H
#define MOUSE_CODES_H

#include <cstdint>

#define OGN_BUTTON_LEFT     1
#define OGN_BUTTON_MIDDLE   2
#define OGN_BUTTON_RIGHT    3
#define OGN_BUTTON_X1       4
#define OGN_BUTTON_X2       5
#define OGN_BUTTON_MASK(X)  (1u << ((X)-1))
#define OGN_BUTTON_LMASK    OGN_BUTTON_MASK(OGN_BUTTON_LEFT)
#define OGN_BUTTON_MMASK    OGN_BUTTON_MASK(OGN_BUTTON_MIDDLE)
#define OGN_BUTTON_RMASK    OGN_BUTTON_MASK(OGN_BUTTON_RIGHT)
#define OGN_BUTTON_X1MASK   OGN_BUTTON_MASK(OGN_BUTTON_X1)
#define OGN_BUTTON_X2MASK   OGN_BUTTON_MASK(OGN_BUTTON_X2)

using MouseCode = u32;
namespace origin::Mouse{
enum : MouseCode
{
	ButtonLeft = OGN_BUTTON_LEFT,
	ButtonRight = OGN_BUTTON_RIGHT,
	ButtonMiddle = OGN_BUTTON_MIDDLE
};
}

#endif
