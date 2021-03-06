/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   hardwarekey.h
 */

#ifndef STMI_HARDWARE_KEY_H
#define STMI_HARDWARE_KEY_H

#include <type_traits>
#include <cstddef>
#include <vector>
#include <functional>

namespace stmi
{

/** The hardware key codes.
 * See <linux/input.h>
 */
enum HARDWARE_KEY {
	HK_NULL = 0
	, HK_ESC = 1
	, HK_1 = 2
	, HK_2 = 3
	, HK_3 = 4
	, HK_4 = 5
	, HK_5 = 6
	, HK_6 = 7
	, HK_7 = 8
	, HK_8 = 9
	, HK_9 = 10
	, HK_0 = 11
	, HK_MINUS = 12
	, HK_EQUAL = 13
	, HK_BACKSPACE = 14
	, HK_TAB = 15
	, HK_Q = 16
	, HK_W = 17
	, HK_E = 18
	, HK_R = 19
	, HK_T = 20
	, HK_Y = 21
	, HK_U = 22
	, HK_I = 23
	, HK_O = 24
	, HK_P = 25
	, HK_LEFTBRACE = 26
	, HK_RIGHTBRACE = 27
	, HK_ENTER = 28
	, HK_LEFTCTRL = 29
	, HK_A = 30
	, HK_S = 31
	, HK_D = 32
	, HK_F = 33
	, HK_G = 34
	, HK_H = 35
	, HK_J = 36
	, HK_K = 37
	, HK_L = 38
	, HK_SEMICOLON = 39
	, HK_APOSTROPHE = 40
	, HK_GRAVE = 41
	, HK_LEFTSHIFT = 42
	, HK_BACKSLASH = 43
	, HK_Z = 44
	, HK_X = 45
	, HK_C = 46
	, HK_V = 47
	, HK_B = 48
	, HK_N = 49
	, HK_M = 50
	, HK_COMMA = 51
	, HK_DOT = 52
	, HK_SLASH = 53
	, HK_RIGHTSHIFT = 54
	, HK_KPASTERISK = 55
	, HK_LEFTALT = 56
	, HK_SPACE = 57
	, HK_CAPSLOCK = 58
	, HK_F1 = 59
	, HK_F2 = 60
	, HK_F3 = 61
	, HK_F4 = 62
	, HK_F5 = 63
	, HK_F6 = 64
	, HK_F7 = 65
	, HK_F8 = 66
	, HK_F9 = 67
	, HK_F10 = 68
	, HK_NUMLOCK = 69
	, HK_SCROLLLOCK = 70
	, HK_KP7 = 71
	, HK_KP8 = 72
	, HK_KP9 = 73
	, HK_KPMINUS = 74
	, HK_KP4 = 75
	, HK_KP5 = 76
	, HK_KP6 = 77
	, HK_KPPLUS = 78
	, HK_KP1 = 79
	, HK_KP2 = 80
	, HK_KP3 = 81
	, HK_KP0 = 82
	, HK_KPDOT = 83

	, HK_ZENKAKUHANKAKU = 85
	, HK_102ND = 86
	, HK_F11 = 87
	, HK_F12 = 88
	, HK_RO = 89
	, HK_KATAKANA = 90
	, HK_HIRAGANA = 91
	, HK_HENKAN = 92
	, HK_KATAKANAHIRAGANA = 93
	, HK_MUHENKAN = 94
	, HK_KPJPCOMMA = 95
	, HK_KPENTER = 96
	, HK_RIGHTCTRL = 97
	, HK_KPSLASH = 98
	, HK_SYSRQ = 99
	, HK_RIGHTALT = 100
	, HK_LINEFEED = 101
	, HK_HOME = 102
	, HK_UP = 103
	, HK_PAGEUP = 104
	, HK_LEFT = 105
	, HK_RIGHT = 106
	, HK_END = 107
	, HK_DOWN = 108
	, HK_PAGEDOWN = 109
	, HK_INSERT = 110
	, HK_DELETE = 111
	, HK_MACRO = 112
	, HK_MUTE = 113
	, HK_VOLUMEDOWN = 114
	, HK_VOLUMEUP = 115
	, HK_POWER = 116
	, HK_KPEQUAL = 117
	, HK_KPPLUSMINUS = 118
	, HK_PAUSE = 119
	, HK_SCALE = 120

	, HK_KPCOMMA = 121
	, HK_HANGEUL = 122
		//, HK_HANGUEL = HK_HANGEUL
	, HK_HANJA = 123
	, HK_YEN = 124
	, HK_LEFTMETA = 125
	, HK_RIGHTMETA = 126
	, HK_COMPOSE = 127

	, HK_STOP = 128
	, HK_AGAIN = 129
	, HK_PROPS = 130
	, HK_UNDO = 131
	, HK_FRONT = 132
	, HK_COPY = 133
	, HK_OPEN = 134
	, HK_PASTE = 135
	, HK_FIND = 136
	, HK_CUT = 137
	, HK_HELP = 138
	, HK_MENU = 139
	, HK_CALC = 140
	, HK_SETUP = 141
	, HK_SLEEP = 142
	, HK_WAKEUP = 143
	, HK_FILE = 144
	, HK_SENDFILE = 145
	, HK_DELETEFILE = 146
	, HK_XFER = 147
	, HK_PROG1 = 148
	, HK_PROG2 = 149
	, HK_WWW = 150
	, HK_MSDOS = 151
	, HK_SCREENLOCK = 152
		//, HK_COFFEE = HK_SCREENLOCK
	, HK_DIRECTION = 153
	, HK_CYCLEWINDOWS = 154
	, HK_MAIL = 155
	, HK_BOOKMARKS = 156
	, HK_COMPUTER = 157
	, HK_BACK = 158
	, HK_FORWARD = 159
	, HK_CLOSECD = 160
	, HK_EJECTCD = 161
	, HK_EJECTCLOSECD = 162
	, HK_NEXTSONG = 163
	, HK_PLAYPAUSE = 164
	, HK_PREVIOUSSONG = 165
	, HK_STOPCD = 166
	, HK_RECORD = 167
	, HK_REWIND = 168
	, HK_PHONE = 169
	, HK_ISO = 170
	, HK_CONFIG = 171
	, HK_HOMEPAGE = 172
	, HK_REFRESH = 173
	, HK_EXIT = 174
	, HK_MOVE = 175
	, HK_EDIT = 176
	, HK_SCROLLUP = 177
	, HK_SCROLLDOWN = 178
	, HK_KPLEFTPAREN = 179
	, HK_KPRIGHTPAREN = 180
	, HK_NEW = 181
	, HK_REDO = 182

	, HK_F13 = 183
	, HK_F14 = 184
	, HK_F15 = 185
	, HK_F16 = 186
	, HK_F17 = 187
	, HK_F18 = 188
	, HK_F19 = 189
	, HK_F20 = 190
	, HK_F21 = 191
	, HK_F22 = 192
	, HK_F23 = 193
	, HK_F24 = 194

	, HK_PLAYCD = 200
	, HK_PAUSECD = 201
	, HK_PROG3 = 202
	, HK_PROG4 = 203
	, HK_DASHBOARD = 204
	, HK_SUSPEND = 205
	, HK_CLOSE = 206
	, HK_PLAY = 207
	, HK_FASTFORWARD = 208
	, HK_BASSBOOST = 209
	, HK_PRINT = 210
	, HK_HP = 211
	, HK_CAMERA = 212
	, HK_SOUND = 213
	, HK_QUESTION = 214
	, HK_EMAIL = 215
	, HK_CHAT = 216
	, HK_SEARCH = 217
	, HK_CONNECT = 218
	, HK_FINANCE = 219
	, HK_SPORT = 220
	, HK_SHOP = 221
	, HK_ALTERASE = 222
	, HK_CANCEL = 223
	, HK_BRIGHTNESSDOWN = 224
	, HK_BRIGHTNESSUP = 225
	, HK_MEDIA = 226

	, HK_SWITCHVIDEOMODE = 227
	, HK_KBDILLUMTOGGLE = 228
	, HK_KBDILLUMDOWN = 229
	, HK_KBDILLUMUP = 230

	, HK_SEND = 231
	, HK_REPLY = 232
	, HK_FORWARDMAIL = 233
	, HK_SAVE = 234
	, HK_DOCUMENTS = 235

	, HK_BATTERY = 236

	, HK_BLUETOOTH = 237
	, HK_WLAN = 238
	, HK_UWB = 239

	, HK_UNKNOWN = 240

	, HK_VIDEO_NEXT = 241
	, HK_VIDEO_PREV = 242
	, HK_BRIGHTNESS_CYCLE = 243
	, HK_BRIGHTNESS_ZERO = 244
		//, HK_BRIGHTNESS_AUTO = HK_BRIGHTNESS_ZERO
	, HK_DISPLAY_OFF = 245

	, HK_WIMAX = 246
		//, HK_WWAN = HK_WIMAX
	, HK_RFKILL = 247

	, HK_MICMUTE = 248

		//, HK_BTN_MISC = 0x100
	, HK_BTN_0 = 0x100
	, HK_BTN_1 = 0x101
	, HK_BTN_2 = 0x102
	, HK_BTN_3 = 0x103
	, HK_BTN_4 = 0x104
	, HK_BTN_5 = 0x105
	, HK_BTN_6 = 0x106
	, HK_BTN_7 = 0x107
	, HK_BTN_8 = 0x108
	, HK_BTN_9 = 0x109

		//, HK_BTN_MOUSE = 0x110
	, HK_BTN_LEFT = 0x110
	, HK_BTN_RIGHT = 0x111
	, HK_BTN_MIDDLE = 0x112
	, HK_BTN_SIDE = 0x113
	, HK_BTN_EXTRA = 0x114
	, HK_BTN_FORWARD = 0x115
	, HK_BTN_BACK = 0x116
	, HK_BTN_TASK = 0x117

		//, HK_BTN_JOYSTICK = 0x120
	, HK_BTN_TRIGGER = 0x120
	, HK_BTN_THUMB = 0x121
	, HK_BTN_THUMB2 = 0x122
	, HK_BTN_TOP = 0x123
	, HK_BTN_TOP2 = 0x124
	, HK_BTN_PINKIE = 0x125
	, HK_BTN_BASE = 0x126
	, HK_BTN_BASE2 = 0x127
	, HK_BTN_BASE3 = 0x128
	, HK_BTN_BASE4 = 0x129
	, HK_BTN_BASE5 = 0x12a
	, HK_BTN_BASE6 = 0x12b
	, HK_BTN_DEAD = 0x12f

		//, HK_BTN_GAMEPAD = 0x130
	, HK_BTN_A = 0x130
		//, HK_BTN_SOUTH = HK_BTN_A
	, HK_BTN_B = 0x131
		//, HK_BTN_EAST = HK_BTN_B
	, HK_BTN_C = 0x132
	, HK_BTN_X = 0x133
		//, HK_BTN_NORTH = HK_BTN_X
	, HK_BTN_Y = 0x134
		//, HK_BTN_WEST = HK_BTN_Y
	, HK_BTN_Z = 0x135
	, HK_BTN_TL = 0x136
	, HK_BTN_TR = 0x137
	, HK_BTN_TL2 = 0x138
	, HK_BTN_TR2 = 0x139
	, HK_BTN_SELECT = 0x13a
	, HK_BTN_START = 0x13b
	, HK_BTN_MODE = 0x13c
	, HK_BTN_THUMBL = 0x13d
	, HK_BTN_THUMBR = 0x13e

		//, HK_BTN_DIGI = 0x140
	, HK_BTN_TOOL_PEN = 0x140
	, HK_BTN_TOOL_RUBBER = 0x141
	, HK_BTN_TOOL_BRUSH = 0x142
	, HK_BTN_TOOL_PENCIL = 0x143
	, HK_BTN_TOOL_AIRBRUSH = 0x144
	, HK_BTN_TOOL_FINGER = 0x145
	, HK_BTN_TOOL_MOUSE = 0x146
	, HK_BTN_TOOL_LENS = 0x147
	, HK_BTN_TOOL_QUINTTAP = 0x148
	, HK_BTN_TOUCH = 0x14a
	, HK_BTN_STYLUS = 0x14b
	, HK_BTN_STYLUS2 = 0x14c
	, HK_BTN_TOOL_DOUBLETAP = 0x14d
	, HK_BTN_TOOL_TRIPLETAP = 0x14e
	, HK_BTN_TOOL_QUADTAP = 0x14f

		//, HK_BTN_WHEEL = 0x150
	, HK_BTN_GEAR_DOWN = 0x150
	, HK_BTN_GEAR_UP = 0x151

	, HK_OK = 0x160
	, HK_SELECT = 0x161
	, HK_GOTO = 0x162
	, HK_CLEAR = 0x163
	, HK_POWER2 = 0x164
	, HK_OPTION = 0x165
	, HK_INFO = 0x166
	, HK_TIME = 0x167
	, HK_VENDOR = 0x168
	, HK_ARCHIVE = 0x169
	, HK_PROGRAM = 0x16a
	, HK_CHANNEL = 0x16b
	, HK_FAVORITES = 0x16c
	, HK_EPG = 0x16d
	, HK_PVR = 0x16e
	, HK_MHP = 0x16f
	, HK_LANGUAGE = 0x170
	, HK_TITLE = 0x171
	, HK_SUBTITLE = 0x172
	, HK_ANGLE = 0x173
	, HK_ZOOM = 0x174
	, HK_MODE = 0x175
	, HK_KEYBOARD = 0x176
	, HK_SCREEN = 0x177
	, HK_PC = 0x178
	, HK_TV = 0x179
	, HK_TV2 = 0x17a
	, HK_VCR = 0x17b
	, HK_VCR2 = 0x17c
	, HK_SAT = 0x17d
	, HK_SAT2 = 0x17e
	, HK_CD = 0x17f
	, HK_TAPE = 0x180
	, HK_RADIO = 0x181
	, HK_TUNER = 0x182
	, HK_PLAYER = 0x183
	, HK_TEXT = 0x184
	, HK_DVD = 0x185
	, HK_AUX = 0x186
	, HK_MP3 = 0x187
	, HK_AUDIO = 0x188
	, HK_VIDEO = 0x189
	, HK_DIRECTORY = 0x18a
	, HK_LIST = 0x18b
	, HK_MEMO = 0x18c
	, HK_CALENDAR = 0x18d
	, HK_RED = 0x18e
	, HK_GREEN = 0x18f
	, HK_YELLOW = 0x190
	, HK_BLUE = 0x191
	, HK_CHANNELUP = 0x192
	, HK_CHANNELDOWN = 0x193
	, HK_FIRST = 0x194
	, HK_LAST = 0x195
	, HK_AB = 0x196
	, HK_NEXT = 0x197
	, HK_RESTART = 0x198
	, HK_SLOW = 0x199
	, HK_SHUFFLE = 0x19a
	, HK_BREAK = 0x19b
	, HK_PREVIOUS = 0x19c
	, HK_DIGITS = 0x19d
	, HK_TEEN = 0x19e
	, HK_TWEN = 0x19f
	, HK_VIDEOPHONE = 0x1a0
	, HK_GAMES = 0x1a1
	, HK_ZOOMIN = 0x1a2
	, HK_ZOOMOUT = 0x1a3
	, HK_ZOOMRESET = 0x1a4
	, HK_WORDPROCESSOR = 0x1a5
	, HK_EDITOR = 0x1a6
	, HK_SPREADSHEET = 0x1a7
	, HK_GRAPHICSEDITOR = 0x1a8
	, HK_PRESENTATION = 0x1a9
	, HK_DATABASE = 0x1aa
	, HK_NEWS = 0x1ab
	, HK_VOICEMAIL = 0x1ac
	, HK_ADDRESSBOOK = 0x1ad
	, HK_MESSENGER = 0x1ae
	, HK_BRIGHTNESS_TOGGLE = 0x1af
		//, HK_DISPLAYTOGGLE = HK_BRIGHTNESS_TOGGLE
	, HK_SPELLCHECK = 0x1b0
	, HK_LOGOFF = 0x1b1

	, HK_DOLLAR = 0x1b2
	, HK_EURO = 0x1b3

	, HK_FRAMEBACK = 0x1b4
	, HK_FRAMEFORWARD = 0x1b5
	, HK_CONTEXT_MENU = 0x1b6
	, HK_MEDIA_REPEAT = 0x1b7
	, HK_10CHANNELSUP = 0x1b8
	, HK_10CHANNELSDOWN = 0x1b9
	, HK_IMAGES = 0x1ba

	, HK_DEL_EOL = 0x1c0
	, HK_DEL_EOS = 0x1c1
	, HK_INS_LINE = 0x1c2
	, HK_DEL_LINE = 0x1c3

	, HK_FN = 0x1d0
	, HK_FN_ESC = 0x1d1
	, HK_FN_F1 = 0x1d2
	, HK_FN_F2 = 0x1d3
	, HK_FN_F3 = 0x1d4
	, HK_FN_F4 = 0x1d5
	, HK_FN_F5 = 0x1d6
	, HK_FN_F6 = 0x1d7
	, HK_FN_F7 = 0x1d8
	, HK_FN_F8 = 0x1d9
	, HK_FN_F9 = 0x1da
	, HK_FN_F10 = 0x1db
	, HK_FN_F11 = 0x1dc
	, HK_FN_F12 = 0x1dd
	, HK_FN_1 = 0x1de
	, HK_FN_2 = 0x1df
	, HK_FN_D = 0x1e0
	, HK_FN_E = 0x1e1
	, HK_FN_F = 0x1e2
	, HK_FN_S = 0x1e3
	, HK_FN_B = 0x1e4

	, HK_BRL_DOT1 = 0x1f1
	, HK_BRL_DOT2 = 0x1f2
	, HK_BRL_DOT3 = 0x1f3
	, HK_BRL_DOT4 = 0x1f4
	, HK_BRL_DOT5 = 0x1f5
	, HK_BRL_DOT6 = 0x1f6
	, HK_BRL_DOT7 = 0x1f7
	, HK_BRL_DOT8 = 0x1f8
	, HK_BRL_DOT9 = 0x1f9
	, HK_BRL_DOT10 = 0x1fa

	, HK_NUMERIC_0 = 0x200
	, HK_NUMERIC_1 = 0x201
	, HK_NUMERIC_2 = 0x202
	, HK_NUMERIC_3 = 0x203
	, HK_NUMERIC_4 = 0x204
	, HK_NUMERIC_5 = 0x205
	, HK_NUMERIC_6 = 0x206
	, HK_NUMERIC_7 = 0x207
	, HK_NUMERIC_8 = 0x208
	, HK_NUMERIC_9 = 0x209
	, HK_NUMERIC_STAR = 0x20a
	, HK_NUMERIC_POUND = 0x20b

	, HK_CAMERA_FOCUS = 0x210
	, HK_WPS_BUTTON = 0x211

	, HK_TOUCHPAD_TOGGLE = 0x212
	, HK_TOUCHPAD_ON = 0x213
	, HK_TOUCHPAD_OFF = 0x214

	, HK_CAMERA_ZOOMIN = 0x215
	, HK_CAMERA_ZOOMOUT = 0x216
	, HK_CAMERA_UP = 0x217
	, HK_CAMERA_DOWN = 0x218
	, HK_CAMERA_LEFT = 0x219
	, HK_CAMERA_RIGHT = 0x21a

	, HK_ATTENDANT_ON = 0x21b
	, HK_ATTENDANT_OFF = 0x21c
	, HK_ATTENDANT_TOGGLE = 0x21d
	, HK_LIGHTS_TOGGLE = 0x21e

	, HK_BTN_DPAD_UP = 0x220
	, HK_BTN_DPAD_DOWN = 0x221
	, HK_BTN_DPAD_LEFT = 0x222
	, HK_BTN_DPAD_RIGHT = 0x223

	, HK_ALS_TOGGLE = 0x230

	, HK_BUTTONCONFIG = 0x240
	, HK_TASKMANAGER = 0x241
	, HK_JOURNAL = 0x242
	, HK_CONTROLPANEL = 0x243
	, HK_APPSELECT = 0x244
	, HK_SCREENSAVER = 0x245
	, HK_VOICECOMMAND = 0x246

	, HK_BRIGHTNESS_MIN = 0x250
	, HK_BRIGHTNESS_MAX = 0x251

	, HK_KBDINPUTASSIST_PREV = 0x260
	, HK_KBDINPUTASSIST_NEXT = 0x261
	, HK_KBDINPUTASSIST_PREVGROUP = 0x262
	, HK_KBDINPUTASSIST_NEXTGROUP = 0x263
	, HK_KBDINPUTASSIST_ACCEPT = 0x264
	, HK_KBDINPUTASSIST_CANCEL = 0x265

		//, HK_BTN_TRIGGER_HAPPY = 0x2c0
	, HK_BTN_TRIGGER_HAPPY1 = 0x2c0
	, HK_BTN_TRIGGER_HAPPY2 = 0x2c1
	, HK_BTN_TRIGGER_HAPPY3 = 0x2c2
	, HK_BTN_TRIGGER_HAPPY4 = 0x2c3
	, HK_BTN_TRIGGER_HAPPY5 = 0x2c4
	, HK_BTN_TRIGGER_HAPPY6 = 0x2c5
	, HK_BTN_TRIGGER_HAPPY7 = 0x2c6
	, HK_BTN_TRIGGER_HAPPY8 = 0x2c7
	, HK_BTN_TRIGGER_HAPPY9 = 0x2c8
	, HK_BTN_TRIGGER_HAPPY10 = 0x2c9
	, HK_BTN_TRIGGER_HAPPY11 = 0x2ca
	, HK_BTN_TRIGGER_HAPPY12 = 0x2cb
	, HK_BTN_TRIGGER_HAPPY13 = 0x2cc
	, HK_BTN_TRIGGER_HAPPY14 = 0x2cd
	, HK_BTN_TRIGGER_HAPPY15 = 0x2ce
	, HK_BTN_TRIGGER_HAPPY16 = 0x2cf
	, HK_BTN_TRIGGER_HAPPY17 = 0x2d0
	, HK_BTN_TRIGGER_HAPPY18 = 0x2d1
	, HK_BTN_TRIGGER_HAPPY19 = 0x2d2
	, HK_BTN_TRIGGER_HAPPY20 = 0x2d3
	, HK_BTN_TRIGGER_HAPPY21 = 0x2d4
	, HK_BTN_TRIGGER_HAPPY22 = 0x2d5
	, HK_BTN_TRIGGER_HAPPY23 = 0x2d6
	, HK_BTN_TRIGGER_HAPPY24 = 0x2d7
	, HK_BTN_TRIGGER_HAPPY25 = 0x2d8
	, HK_BTN_TRIGGER_HAPPY26 = 0x2d9
	, HK_BTN_TRIGGER_HAPPY27 = 0x2da
	, HK_BTN_TRIGGER_HAPPY28 = 0x2db
	, HK_BTN_TRIGGER_HAPPY29 = 0x2dc
	, HK_BTN_TRIGGER_HAPPY30 = 0x2dd
	, HK_BTN_TRIGGER_HAPPY31 = 0x2de
	, HK_BTN_TRIGGER_HAPPY32 = 0x2df
	, HK_BTN_TRIGGER_HAPPY33 = 0x2e0
	, HK_BTN_TRIGGER_HAPPY34 = 0x2e1
	, HK_BTN_TRIGGER_HAPPY35 = 0x2e2
	, HK_BTN_TRIGGER_HAPPY36 = 0x2e3
	, HK_BTN_TRIGGER_HAPPY37 = 0x2e4
	, HK_BTN_TRIGGER_HAPPY38 = 0x2e5
	, HK_BTN_TRIGGER_HAPPY39 = 0x2e6
	, HK_BTN_TRIGGER_HAPPY40 = 0x2e7

	, HK_X_SCROLL_LEFT = 0x300 /**< this is an extra definition not present in <linux/input.h> */
	, HK_X_SCROLL_RIGHT = 0x301 /**< this is an extra definition not present in <linux/input.h> */
};

} // namespace stmi

namespace std {
	template <>
	struct hash<stmi::HARDWARE_KEY> : private hash< std::underlying_type<stmi::HARDWARE_KEY>::type >
	{
		std::size_t operator()(const stmi::HARDWARE_KEY& eKey) const noexcept
		{
			return hash< std::underlying_type<stmi::HARDWARE_KEY>::type >::operator()
					(static_cast<std::underlying_type<stmi::HARDWARE_KEY>::type>(eKey));
		}
	};
} // namespace std

namespace stmi
{

namespace HardwareKeys
{

/** Returns the defined HARDWARE_KEY values as an ordered vector (`HK_NULL` excluded).
 * The singleton vector is only created when this function is first called.
 * @return The hardware keys as a std::vector.
 */
const std::vector<HARDWARE_KEY>& get() noexcept;
/** Tells whether a key is valid.
 * `HK_NULL` is not considered a valid key by this function!
 * @param eKey The key to check.
 * @return Whether the key is valid.
 */
bool isValid(HARDWARE_KEY eKey) noexcept;

} // namespace HardwareKeys

} // namespace stmi

#endif /* STMI_HARDWARE_KEY_H */
