// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#ifndef KEY_CODES_H
#define KEY_CODES_H

#include <cstdint>

#define OGN_KEY_EXTENDED_MASK          (1u << 29)
#define OGN_KEY_SCANCODE_MASK          (1u << 30)
#define OGN_KEY_SCANCODE_TO_KEYCODE(X) (X | OGN_KEY_SCANCODE_MASK)
#define OGN_KEY_UNKNOWN                0x00000000u /**< 0 */
#define OGN_KEY_RETURN                 0x0000000du /**< '\r' */
#define OGN_KEY_ESCAPE                 0x0000001bu /**< '\x1B' */
#define OGN_KEY_BACKSPACE              0x00000008u /**< '\b' */
#define OGN_KEY_TAB                    0x00000009u /**< '\t' */
#define OGN_KEY_SPACE                  0x00000020u /**< ' ' */
#define OGN_KEY_EXCLAIM                0x00000021u /**< '!' */
#define OGN_KEY_DBLAPOSTROPHE          0x00000022u /**< '"' */
#define OGN_KEY_HASH                   0x00000023u /**< '#' */
#define OGN_KEY_DOLLAR                 0x00000024u /**< '$' */
#define OGN_KEY_PERCENT                0x00000025u /**< '%' */
#define OGN_KEY_AMPERSAND              0x00000026u /**< '&' */
#define OGN_KEY_APOSTROPHE             0x00000027u /**< '\'' */
#define OGN_KEY_LEFTPAREN              0x00000028u /**< '(' */
#define OGN_KEY_RIGHTPAREN             0x00000029u /**< ')' */
#define OGN_KEY_ASTERISK               0x0000002au /**< '*' */
#define OGN_KEY_PLUS                   0x0000002bu /**< '+' */
#define OGN_KEY_COMMA                  0x0000002cu /**< ',' */
#define OGN_KEY_MINUS                  0x0000002du /**< '-' */
#define OGN_KEY_PERIOD                 0x0000002eu /**< '.' */
#define OGN_KEY_SLASH                  0x0000002fu /**< '/' */
#define OGN_KEY_0                      0x00000030u /**< '0' */
#define OGN_KEY_1                      0x00000031u /**< '1' */
#define OGN_KEY_2                      0x00000032u /**< '2' */
#define OGN_KEY_3                      0x00000033u /**< '3' */
#define OGN_KEY_4                      0x00000034u /**< '4' */
#define OGN_KEY_5                      0x00000035u /**< '5' */
#define OGN_KEY_6                      0x00000036u /**< '6' */
#define OGN_KEY_7                      0x00000037u /**< '7' */
#define OGN_KEY_8                      0x00000038u /**< '8' */
#define OGN_KEY_9                      0x00000039u /**< '9' */
#define OGN_KEY_COLON                  0x0000003au /**< ':' */
#define OGN_KEY_SEMICOLON              0x0000003bu /**< ';' */
#define OGN_KEY_LESS                   0x0000003cu /**< '<' */
#define OGN_KEY_EQUALS                 0x0000003du /**< '=' */
#define OGN_KEY_GREATER                0x0000003eu /**< '>' */
#define OGN_KEY_QUESTION               0x0000003fu /**< '?' */
#define OGN_KEY_AT                     0x00000040u /**< '@' */
#define OGN_KEY_LEFTBRACKET            0x0000005bu /**< '[' */
#define OGN_KEY_BACKSLASH              0x0000005cu /**< '\\' */
#define OGN_KEY_RIGHTBRACKET           0x0000005du /**< ']' */
#define OGN_KEY_CARET                  0x0000005eu /**< '^' */
#define OGN_KEY_UNDERSCORE             0x0000005fu /**< '_' */
#define OGN_KEY_GRAVE                  0x00000060u /**< '`' */
#define OGN_KEY_A                      0x00000061u /**< 'a' */
#define OGN_KEY_B                      0x00000062u /**< 'b' */
#define OGN_KEY_C                      0x00000063u /**< 'c' */
#define OGN_KEY_D                      0x00000064u /**< 'd' */
#define OGN_KEY_E                      0x00000065u /**< 'e' */
#define OGN_KEY_F                      0x00000066u /**< 'f' */
#define OGN_KEY_G                      0x00000067u /**< 'g' */
#define OGN_KEY_H                      0x00000068u /**< 'h' */
#define OGN_KEY_I                      0x00000069u /**< 'i' */
#define OGN_KEY_J                      0x0000006au /**< 'j' */
#define OGN_KEY_K                      0x0000006bu /**< 'k' */
#define OGN_KEY_L                      0x0000006cu /**< 'l' */
#define OGN_KEY_M                      0x0000006du /**< 'm' */
#define OGN_KEY_N                      0x0000006eu /**< 'n' */
#define OGN_KEY_O                      0x0000006fu /**< 'o' */
#define OGN_KEY_P                      0x00000070u /**< 'p' */
#define OGN_KEY_Q                      0x00000071u /**< 'q' */
#define OGN_KEY_R                      0x00000072u /**< 'r' */
#define OGN_KEY_S                      0x00000073u /**< 's' */
#define OGN_KEY_T                      0x00000074u /**< 't' */
#define OGN_KEY_U                      0x00000075u /**< 'u' */
#define OGN_KEY_V                      0x00000076u /**< 'v' */
#define OGN_KEY_W                      0x00000077u /**< 'w' */
#define OGN_KEY_X                      0x00000078u /**< 'x' */
#define OGN_KEY_Y                      0x00000079u /**< 'y' */
#define OGN_KEY_Z                      0x0000007au /**< 'z' */
#define OGN_KEY_LEFTBRACE              0x0000007bu /**< '{' */
#define OGN_KEY_PIPE                   0x0000007cu /**< '|' */
#define OGN_KEY_RIGHTBRACE             0x0000007du /**< '}' */
#define OGN_KEY_TILDE                  0x0000007eu /**< '~' */
#define OGN_KEY_DELETE                 0x0000007fu /**< '\x7F' */
#define OGN_KEY_PLUSMINUS              0x000000b1u /**< '\xB1' */
#define OGN_KEY_CAPSLOCK               0x40000039u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CAPSLOCK) */
#define OGN_KEY_F1                     0x4000003au /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F1) */
#define OGN_KEY_F2                     0x4000003bu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F2) */
#define OGN_KEY_F3                     0x4000003cu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F3) */
#define OGN_KEY_F4                     0x4000003du /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F4) */
#define OGN_KEY_F5                     0x4000003eu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F5) */
#define OGN_KEY_F6                     0x4000003fu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F6) */
#define OGN_KEY_F7                     0x40000040u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F7) */
#define OGN_KEY_F8                     0x40000041u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F8) */
#define OGN_KEY_F9                     0x40000042u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F9) */
#define OGN_KEY_F10                    0x40000043u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F10) */
#define OGN_KEY_F11                    0x40000044u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F11) */
#define OGN_KEY_F12                    0x40000045u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F12) */
#define OGN_KEY_PRINTSCREEN            0x40000046u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRINTSCREEN) */
#define OGN_KEY_SCROLLLOCK             0x40000047u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SCROLLLOCK) */
#define OGN_KEY_PAUSE                  0x40000048u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAUSE) */
#define OGN_KEY_INSERT                 0x40000049u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_INSERT) */
#define OGN_KEY_HOME                   0x4000004au /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HOME) */
#define OGN_KEY_PAGEUP                 0x4000004bu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEUP) */
#define OGN_KEY_END                    0x4000004du /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_END) */
#define OGN_KEY_PAGEDOWN               0x4000004eu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEDOWN) */
#define OGN_KEY_RIGHT                  0x4000004fu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RIGHT) */
#define OGN_KEY_LEFT                   0x40000050u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LEFT) */
#define OGN_KEY_DOWN                   0x40000051u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DOWN) */
#define OGN_KEY_UP                     0x40000052u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UP) */
#define OGN_KEY_NUMLOCKCLEAR           0x40000053u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_NUMLOCKCLEAR) */
#define OGN_KEY_KP_DIVIDE              0x40000054u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DIVIDE) */
#define OGN_KEY_KP_MULTIPLY            0x40000055u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MULTIPLY) */
#define OGN_KEY_KP_MINUS               0x40000056u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MINUS) */
#define OGN_KEY_KP_PLUS                0x40000057u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUS) */
#define OGN_KEY_KP_ENTER               0x40000058u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_ENTER) */
#define OGN_KEY_KP_1                   0x40000059u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_1) */
#define OGN_KEY_KP_2                   0x4000005au /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_2) */
#define OGN_KEY_KP_3                   0x4000005bu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_3) */
#define OGN_KEY_KP_4                   0x4000005cu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_4) */
#define OGN_KEY_KP_5                   0x4000005du /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_5) */
#define OGN_KEY_KP_6                   0x4000005eu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_6) */
#define OGN_KEY_KP_7                   0x4000005fu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_7) */
#define OGN_KEY_KP_8                   0x40000060u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_8) */
#define OGN_KEY_KP_9                   0x40000061u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_9) */
#define OGN_KEY_KP_0                   0x40000062u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_0) */
#define OGN_KEY_KP_PERIOD              0x40000063u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERIOD) */
#define OGN_KEY_APPLICATION            0x40000065u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APPLICATION) */
#define OGN_KEY_POWER                  0x40000066u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_POWER) */
#define OGN_KEY_KP_EQUALS              0x40000067u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EQUALS) */
#define OGN_KEY_F13                    0x40000068u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F13) */
#define OGN_KEY_F14                    0x40000069u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F14) */
#define OGN_KEY_F15                    0x4000006au /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F15) */
#define OGN_KEY_F16                    0x4000006bu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F16) */
#define OGN_KEY_F17                    0x4000006cu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F17) */
#define OGN_KEY_F18                    0x4000006du /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F18) */
#define OGN_KEY_F19                    0x4000006eu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F19) */
#define OGN_KEY_F20                    0x4000006fu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F20) */
#define OGN_KEY_F21                    0x40000070u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F21) */
#define OGN_KEY_F22                    0x40000071u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F22) */
#define OGN_KEY_F23                    0x40000072u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F23) */
#define OGN_KEY_F24                    0x40000073u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F24) */
#define OGN_KEY_EXECUTE                0x40000074u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXECUTE) */
#define OGN_KEY_HELP                   0x40000075u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HELP) */
#define OGN_KEY_MENU                   0x40000076u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MENU) */
#define OGN_KEY_SELECT                 0x40000077u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SELECT) */
#define OGN_KEY_STOP                   0x40000078u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_STOP) */
#define OGN_KEY_AGAIN                  0x40000079u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AGAIN) */
#define OGN_KEY_UNDO                   0x4000007au /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UNDO) */
#define OGN_KEY_CUT                    0x4000007bu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CUT) */
#define OGN_KEY_COPY                   0x4000007cu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_COPY) */
#define OGN_KEY_PASTE                  0x4000007du /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PASTE) */
#define OGN_KEY_FIND                   0x4000007eu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_FIND) */
#define OGN_KEY_MUTE                   0x4000007fu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MUTE) */
#define OGN_KEY_VOLUMEUP               0x40000080u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_VOLUMEUP) */
#define OGN_KEY_VOLUMEDOWN             0x40000081u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_VOLUMEDOWN) */
#define OGN_KEY_KP_COMMA               0x40000085u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COMMA) */
#define OGN_KEY_KP_EQUALSAS400         0x40000086u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EQUALSAS400) */
#define OGN_KEY_ALTERASE               0x40000099u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ALTERASE) */
#define OGN_KEY_SYSREQ                 0x4000009au /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SYSREQ) */
#define OGN_KEY_CANCEL                 0x4000009bu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CANCEL) */
#define OGN_KEY_CLEAR                  0x4000009cu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEAR) */
#define OGN_KEY_PRIOR                  0x4000009du /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRIOR) */
#define OGN_KEY_RETURN2                0x4000009eu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RETURN2) */
#define OGN_KEY_SEPARATOR              0x4000009fu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SEPARATOR) */
#define OGN_KEY_OUT                    0x400000a0u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_OUT) */
#define OGN_KEY_OPER                   0x400000a1u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_OPER) */
#define OGN_KEY_CLEARAGAIN             0x400000a2u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEARAGAIN) */
#define OGN_KEY_CRSEL                  0x400000a3u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CRSEL) */
#define OGN_KEY_EXSEL                  0x400000a4u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXSEL) */
#define OGN_KEY_KP_00                  0x400000b0u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_00) */
#define OGN_KEY_KP_000                 0x400000b1u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_000) */
#define OGN_KEY_THOUSANDSSEPARATOR     0x400000b2u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_THOUSANDSSEPARATOR) */
#define OGN_KEY_DECIMALSEPARATOR       0x400000b3u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DECIMALSEPARATOR) */
#define OGN_KEY_CURRENCYUNIT           0x400000b4u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYUNIT) */
#define OGN_KEY_CURRENCYSUBUNIT        0x400000b5u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CURRENCYSUBUNIT) */
#define OGN_KEY_KP_LEFTPAREN           0x400000b6u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTPAREN) */
#define OGN_KEY_KP_RIGHTPAREN          0x400000b7u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTPAREN) */
#define OGN_KEY_KP_LEFTBRACE           0x400000b8u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LEFTBRACE) */
#define OGN_KEY_KP_RIGHTBRACE          0x400000b9u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_RIGHTBRACE) */
#define OGN_KEY_KP_TAB                 0x400000bau /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_TAB) */
#define OGN_KEY_KP_BACKSPACE           0x400000bbu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BACKSPACE) */
#define OGN_KEY_KP_A                   0x400000bcu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_A) */
#define OGN_KEY_KP_B                   0x400000bdu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_B) */
#define OGN_KEY_KP_C                   0x400000beu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_C) */
#define OGN_KEY_KP_D                   0x400000bfu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_D) */
#define OGN_KEY_KP_E                   0x400000c0u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_E) */
#define OGN_KEY_KP_F                   0x400000c1u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_F) */
#define OGN_KEY_KP_XOR                 0x400000c2u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_XOR) */
#define OGN_KEY_KP_POWER               0x400000c3u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_POWER) */
#define OGN_KEY_KP_PERCENT             0x400000c4u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERCENT) */
#define OGN_KEY_KP_LESS                0x400000c5u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_LESS) */
#define OGN_KEY_KP_GREATER             0x400000c6u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_GREATER) */
#define OGN_KEY_KP_AMPERSAND           0x400000c7u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AMPERSAND) */
#define OGN_KEY_KP_DBLAMPERSAND        0x400000c8u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLAMPERSAND) */
#define OGN_KEY_KP_VERTICALBAR         0x400000c9u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_VERTICALBAR) */
#define OGN_KEY_KP_DBLVERTICALBAR      0x400000cau /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DBLVERTICALBAR) */
#define OGN_KEY_KP_COLON               0x400000cbu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_COLON) */
#define OGN_KEY_KP_HASH                0x400000ccu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HASH) */
#define OGN_KEY_KP_SPACE               0x400000cdu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_SPACE) */
#define OGN_KEY_KP_AT                  0x400000ceu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_AT) */
#define OGN_KEY_KP_EXCLAM              0x400000cfu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_EXCLAM) */
#define OGN_KEY_KP_MEMSTORE            0x400000d0u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSTORE) */
#define OGN_KEY_KP_MEMRECALL           0x400000d1u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMRECALL) */
#define OGN_KEY_KP_MEMCLEAR            0x400000d2u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMCLEAR) */
#define OGN_KEY_KP_MEMADD              0x400000d3u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMADD) */
#define OGN_KEY_KP_MEMSUBTRACT         0x400000d4u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMSUBTRACT) */
#define OGN_KEY_KP_MEMMULTIPLY         0x400000d5u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMMULTIPLY) */
#define OGN_KEY_KP_MEMDIVIDE           0x400000d6u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MEMDIVIDE) */
#define OGN_KEY_KP_PLUSMINUS           0x400000d7u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUSMINUS) */
#define OGN_KEY_KP_CLEAR               0x400000d8u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEAR) */
#define OGN_KEY_KP_CLEARENTRY          0x400000d9u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_CLEARENTRY) */
#define OGN_KEY_KP_BINARY              0x400000dau /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_BINARY) */
#define OGN_KEY_KP_OCTAL               0x400000dbu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_OCTAL) */
#define OGN_KEY_KP_DECIMAL             0x400000dcu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DECIMAL) */
#define OGN_KEY_KP_HEXADECIMAL         0x400000ddu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_HEXADECIMAL) */
#define OGN_KEY_LCTRL                  0x400000e0u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL) */
#define OGN_KEY_LSHIFT                 0x400000e1u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT) */
#define OGN_KEY_LALT                   0x400000e2u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT) */
#define OGN_KEY_LGUI                   0x400000e3u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LGUI) */
#define OGN_KEY_RCTRL                  0x400000e4u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL) */
#define OGN_KEY_RSHIFT                 0x400000e5u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT) */
#define OGN_KEY_RALT                   0x400000e6u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT) */
#define OGN_KEY_RGUI                   0x400000e7u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RGUI) */
#define OGN_KEY_MODE                   0x40000101u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MODE) */
#define OGN_KEY_SLEEP                  0x40000102u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SLEEP) */
#define OGN_KEY_WAKE                   0x40000103u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_WAKE) */
#define OGN_KEY_CHANNEL_INCREMENT      0x40000104u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CHANNEL_INCREMENT) */
#define OGN_KEY_CHANNEL_DECREMENT      0x40000105u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CHANNEL_DECREMENT) */
#define OGN_KEY_MEDIA_PLAY             0x40000106u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PLAY) */
#define OGN_KEY_MEDIA_PAUSE            0x40000107u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PAUSE) */
#define OGN_KEY_MEDIA_RECORD           0x40000108u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_RECORD) */
#define OGN_KEY_MEDIA_FAST_FORWARD     0x40000109u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_FAST_FORWARD) */
#define OGN_KEY_MEDIA_REWIND           0x4000010au /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_REWIND) */
#define OGN_KEY_MEDIA_NEXT_TRACK       0x4000010bu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_NEXT_TRACK) */
#define OGN_KEY_MEDIA_PREVIOUS_TRACK   0x4000010cu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PREVIOUS_TRACK) */
#define OGN_KEY_MEDIA_STOP             0x4000010du /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_STOP) */
#define OGN_KEY_MEDIA_EJECT            0x4000010eu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_EJECT) */
#define OGN_KEY_MEDIA_PLAY_PAUSE       0x4000010fu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_PLAY_PAUSE) */
#define OGN_KEY_MEDIA_SELECT           0x40000110u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MEDIA_SELECT) */
#define OGN_KEY_AC_NEW                 0x40000111u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_NEW) */
#define OGN_KEY_AC_OPEN                0x40000112u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_OPEN) */
#define OGN_KEY_AC_CLOSE               0x40000113u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_CLOSE) */
#define OGN_KEY_AC_EXIT                0x40000114u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_EXIT) */
#define OGN_KEY_AC_SAVE                0x40000115u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SAVE) */
#define OGN_KEY_AC_PRINT               0x40000116u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_PRINT) */
#define OGN_KEY_AC_PROPERTIES          0x40000117u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_PROPERTIES) */
#define OGN_KEY_AC_SEARCH              0x40000118u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_SEARCH) */
#define OGN_KEY_AC_HOME                0x40000119u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_HOME) */
#define OGN_KEY_AC_BACK                0x4000011au /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BACK) */
#define OGN_KEY_AC_FORWARD             0x4000011bu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_FORWARD) */
#define OGN_KEY_AC_STOP                0x4000011cu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_STOP) */
#define OGN_KEY_AC_REFRESH             0x4000011du /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_REFRESH) */
#define OGN_KEY_AC_BOOKMARKS           0x4000011eu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_AC_BOOKMARKS) */
#define OGN_KEY_SOFTLEFT               0x4000011fu /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTLEFT) */
#define OGN_KEY_SOFTRIGHT              0x40000120u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SOFTRIGHT) */
#define OGN_KEY_CALL                   0x40000121u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CALL) */
#define OGN_KEY_ENDCALL                0x40000122u /**< SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_ENDCALL) */
#define OGN_KEY_LEFT_TAB               0x20000001u /**< Extended key Left Tab */
#define OGN_KEY_LEVEL5_SHIFT           0x20000002u /**< Extended key Level 5 Shift */
#define OGN_KEY_MULTI_KEY_COMPOSE      0x20000003u /**< Extended key Multi-key Compose */
#define OGN_KEY_LMETA                  0x20000004u /**< Extended key Left Meta */
#define OGN_KEY_RMETA                  0x20000005u /**< Extended key Right Meta */
#define OGN_KEY_LHYPER                 0x20000006u /**< Extended key Left Hyper */
#define OGN_KEY_RHYPER                 0x20000007u /**< Extended key Right Hyper */

#define OGN_KMOD_NONE   0x0000u /**< no modifier is applicable. */
#define OGN_KMOD_LSHIFT 0x0001u /**< the left Shift key is down. */
#define OGN_KMOD_RSHIFT 0x0002u /**< the right Shift key is down. */
#define OGN_KMOD_LEVEL5 0x0004u /**< the Level 5 Shift key is down. */
#define OGN_KMOD_LCTRL  0x0040u /**< the left Ctrl (Control) key is down. */
#define OGN_KMOD_RCTRL  0x0080u /**< the right Ctrl (Control) key is down. */
#define OGN_KMOD_LALT   0x0100u /**< the left Alt key is down. */
#define OGN_KMOD_RALT   0x0200u /**< the right Alt key is down. */
#define OGN_KMOD_LGUI   0x0400u /**< the left GUI key (often the Windows key) is down. */
#define OGN_KMOD_RGUI   0x0800u /**< the right GUI key (often the Windows key) is down. */
#define OGN_KMOD_NUM    0x1000u /**< the Num Lock key (may be located on an extended keypad) is down. */
#define OGN_KMOD_CAPS   0x2000u /**< the Caps Lock key is down. */
#define OGN_KMOD_MODE   0x4000u /**< the !AltGr key is down. */
#define OGN_KMOD_SCROLL 0x8000u /**< the Scroll Lock key is down. */
#define OGN_KMOD_CTRL   (OGN_KMOD_LCTRL | OGN_KMOD_RCTRL)   /**< Any Ctrl key is down. */
#define OGN_KMOD_SHIFT  (OGN_KMOD_LSHIFT | OGN_KMOD_RSHIFT) /**< Any Shift key is down. */
#define OGN_KMOD_ALT    (OGN_KMOD_LALT | OGN_KMOD_RALT)     /**< Any Alt key is down. */
#define OGN_KMOD_GUI    (OGN_KMOD_LGUI | OGN_KMOD_RGUI)     /**< Any GUI key is down. */

using KeyCode = u32;
using KeyModCode = u16;

namespace origin {
namespace Key {
enum : KeyCode
{
    Space = OGN_KEY_SPACE,
    Apostrophe = OGN_KEY_APOSTROPHE, /* ' */
    Comma = OGN_KEY_COMMA, /* , */
    Minus = OGN_KEY_MINUS, /* - */
    Period = OGN_KEY_PERIOD, /* . */
    Slash = OGN_KEY_SLASH, /* / */

    D0 = OGN_KEY_0, /* 0 */
    D1 = OGN_KEY_1, /* 1 */
    D2 = OGN_KEY_2, /* 2 */
    D3 = OGN_KEY_3, /* 3 */
    D4 = OGN_KEY_4, /* 4 */
    D5 = OGN_KEY_5, /* 5 */
    D6 = OGN_KEY_6, /* 6 */
    D7 = OGN_KEY_7, /* 7 */
    D8 = OGN_KEY_8, /* 8 */
    D9 = OGN_KEY_9, /* 9 */

    Semicolon = OGN_KEY_SEMICOLON, /* ; */
    Equal = OGN_KEY_EQUALS, /* = */

    A = OGN_KEY_A,
    B = OGN_KEY_B,
    C = OGN_KEY_C,
    D = OGN_KEY_D,
    E = OGN_KEY_E,
    F = OGN_KEY_F,
    G = OGN_KEY_G,
    H = OGN_KEY_H,
    I = OGN_KEY_I,
    J = OGN_KEY_J,
    K = OGN_KEY_K,
    L = OGN_KEY_L,
    M = OGN_KEY_M,
    N = OGN_KEY_N,
    O = OGN_KEY_O,
    P = OGN_KEY_P,
    Q = OGN_KEY_Q,
    R = OGN_KEY_R,
    S = OGN_KEY_S,
    T = OGN_KEY_T,
    U = OGN_KEY_U,
    V = OGN_KEY_V,
    W = OGN_KEY_W,
    X = OGN_KEY_X,
    Y = OGN_KEY_Y,
    Z = OGN_KEY_Z,

    LeftBracket = OGN_KEY_LEFTBRACKET,  /* [ */
    Backslash = OGN_KEY_BACKSLASH,  /* \ */
    RightBracket = OGN_KEY_RIGHTBRACKET,  /* ] */
    GraveAccent = OGN_KEY_GRAVE,  /* ` */

    /* Function keys */
    Escape = OGN_KEY_ESCAPE,
    Enter = OGN_KEY_RETURN,
    Tab = OGN_KEY_TAB,
    Backspace = OGN_KEY_BACKSPACE,
    Insert = OGN_KEY_INSERT,
    Delete = OGN_KEY_DELETE,
    Right = OGN_KEY_RIGHT,
    Left = OGN_KEY_LEFT,
    Down = OGN_KEY_DOWN,
    Up = OGN_KEY_UP,
    PageUp = OGN_KEY_PAGEUP,
    PageDown = OGN_KEY_PAGEDOWN,
    Home = OGN_KEY_HOME,
    End = OGN_KEY_END,
    CapsLock = OGN_KEY_CAPSLOCK,
    ScrollLock = OGN_KEY_SCROLLLOCK,
    NumLock = OGN_KEY_NUMLOCKCLEAR,
    PrintScreen = OGN_KEY_PRINTSCREEN,
    Pause = OGN_KEY_PAUSE,
    F1 = OGN_KEY_F1,
    F2 = OGN_KEY_F2,
    F3 = OGN_KEY_F3,
    F4 = OGN_KEY_F4,
    F5 = OGN_KEY_F5,
    F6 = OGN_KEY_F6,
    F7 = OGN_KEY_F7,
    F8 = OGN_KEY_F8,
    F9 = OGN_KEY_F9,
    F10 = OGN_KEY_F10,
    F11 = OGN_KEY_F11,
    F12 = OGN_KEY_F12,
    F13 = OGN_KEY_F13,
    F14 = OGN_KEY_F14,
    F15 = OGN_KEY_F15,
    F16 = OGN_KEY_F16,
    F17 = OGN_KEY_F17,
    F18 = OGN_KEY_F18,
    F19 = OGN_KEY_F19,
    F20 = OGN_KEY_F20,
    F21 = OGN_KEY_F21,
    F22 = OGN_KEY_F22,
    F23 = OGN_KEY_F23,
    F24 = OGN_KEY_F24,

    /* Keypad */
    KP0 = OGN_KEY_KP_0,
    KP1 = OGN_KEY_KP_1,
    KP2 = OGN_KEY_KP_2,
    KP3 = OGN_KEY_KP_3,
    KP4 = OGN_KEY_KP_4,
    KP5 = OGN_KEY_KP_5,
    KP6 = OGN_KEY_KP_6,
    KP7 = OGN_KEY_KP_7,
    KP8 = OGN_KEY_KP_8,
    KP9 = OGN_KEY_KP_9,
    KPDecimal = OGN_KEY_KP_DECIMAL,
    KPDivide = OGN_KEY_KP_DIVIDE,
    KPMultiply = OGN_KEY_KP_MULTIPLY,
    KPSubtract = OGN_KEY_KP_MEMSUBTRACT,
    KPAdd = OGN_KEY_KP_MEMADD,
    KPEnter = OGN_KEY_KP_ENTER,
    KPEqual = OGN_KEY_KP_EQUALS,
    Menu = OGN_KEY_MENU,
};
}
namespace KeyMod {
enum : KeyModCode {
    None = OGN_KMOD_NONE,
    Shift = OGN_KMOD_SHIFT,
    Control = OGN_KMOD_CTRL,
    LeftShift = OGN_KMOD_LSHIFT,
    LeftControl = OGN_KMOD_LCTRL,
    LeftAlt = OGN_KMOD_LALT,
    LeftSuper = OGN_KMOD_LGUI,
    RightShift = OGN_KMOD_RSHIFT,
    RightControl = OGN_KMOD_RCTRL,
    RightAlt = OGN_KMOD_RALT,
    RightSuper = OGN_KMOD_RGUI,
    Super = OGN_KMOD_GUI,
};
}
}

#endif