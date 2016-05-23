/*
 * Copyright © 2016  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   hardwarekey.cc
 */

#include "hardwarekey.h"

namespace stmi
{

namespace HardwareKeys
{

const std::unordered_set<HARDWARE_KEY>& get()
{
	// Generate:
	// copy all HARDWARE_KEY enums from hardwarekey.h
	// Add ', ' before HK_ESC (the first enum)
	// transform each line:
	//	, HK_XXX = n //yyyy
	// to:
	//	, HK_XXX
	// with:
	//   Find:     <^	, HK_([^ ]+).*$>
	//   Replace:  <	, HK_\1>

	static const std::unordered_set<HARDWARE_KEY> s_oSet{
	HK_ESC
	, HK_1
	, HK_2
	, HK_3
	, HK_4
	, HK_5
	, HK_6
	, HK_7
	, HK_8
	, HK_9
	, HK_0
	, HK_MINUS
	, HK_EQUAL
	, HK_BACKSPACE
	, HK_TAB
	, HK_Q
	, HK_W
	, HK_E
	, HK_R
	, HK_T
	, HK_Y
	, HK_U
	, HK_I
	, HK_O
	, HK_P
	, HK_LEFTBRACE
	, HK_RIGHTBRACE
	, HK_ENTER
	, HK_LEFTCTRL
	, HK_A
	, HK_S
	, HK_D
	, HK_F
	, HK_G
	, HK_H
	, HK_J
	, HK_K
	, HK_L
	, HK_SEMICOLON
	, HK_APOSTROPHE
	, HK_GRAVE
	, HK_LEFTSHIFT
	, HK_BACKSLASH
	, HK_Z
	, HK_X
	, HK_C
	, HK_V
	, HK_B
	, HK_N
	, HK_M
	, HK_COMMA
	, HK_DOT
	, HK_SLASH
	, HK_RIGHTSHIFT
	, HK_KPASTERISK
	, HK_LEFTALT
	, HK_SPACE
	, HK_CAPSLOCK
	, HK_F1
	, HK_F2
	, HK_F3
	, HK_F4
	, HK_F5
	, HK_F6
	, HK_F7
	, HK_F8
	, HK_F9
	, HK_F10
	, HK_NUMLOCK
	, HK_SCROLLLOCK
	, HK_KP7
	, HK_KP8
	, HK_KP9
	, HK_KPMINUS
	, HK_KP4
	, HK_KP5
	, HK_KP6
	, HK_KPPLUS
	, HK_KP1
	, HK_KP2
	, HK_KP3
	, HK_KP0
	, HK_KPDOT

	, HK_ZENKAKUHANKAKU
	, HK_102ND
	, HK_F11
	, HK_F12
	, HK_RO
	, HK_KATAKANA
	, HK_HIRAGANA
	, HK_HENKAN
	, HK_KATAKANAHIRAGANA
	, HK_MUHENKAN
	, HK_KPJPCOMMA
	, HK_KPENTER
	, HK_RIGHTCTRL
	, HK_KPSLASH
	, HK_SYSRQ
	, HK_RIGHTALT
	, HK_LINEFEED
	, HK_HOME
	, HK_UP
	, HK_PAGEUP
	, HK_LEFT
	, HK_RIGHT
	, HK_END
	, HK_DOWN
	, HK_PAGEDOWN
	, HK_INSERT
	, HK_DELETE
	, HK_MACRO
	, HK_MUTE
	, HK_VOLUMEDOWN
	, HK_VOLUMEUP
	, HK_POWER
	, HK_KPEQUAL
	, HK_KPPLUSMINUS
	, HK_PAUSE
	, HK_SCALE

	, HK_KPCOMMA
	, HK_HANGEUL
	, HK_HANJA
	, HK_YEN
	, HK_LEFTMETA
	, HK_RIGHTMETA
	, HK_COMPOSE

	, HK_STOP
	, HK_AGAIN
	, HK_PROPS
	, HK_UNDO
	, HK_FRONT
	, HK_COPY
	, HK_OPEN
	, HK_PASTE
	, HK_FIND
	, HK_CUT
	, HK_HELP
	, HK_MENU
	, HK_CALC
	, HK_SETUP
	, HK_SLEEP
	, HK_WAKEUP
	, HK_FILE
	, HK_SENDFILE
	, HK_DELETEFILE
	, HK_XFER
	, HK_PROG1
	, HK_PROG2
	, HK_WWW
	, HK_MSDOS
	, HK_SCREENLOCK
	, HK_DIRECTION
	, HK_CYCLEWINDOWS
	, HK_MAIL
	, HK_BOOKMARKS
	, HK_COMPUTER
	, HK_BACK
	, HK_FORWARD
	, HK_CLOSECD
	, HK_EJECTCD
	, HK_EJECTCLOSECD
	, HK_NEXTSONG
	, HK_PLAYPAUSE
	, HK_PREVIOUSSONG
	, HK_STOPCD
	, HK_RECORD
	, HK_REWIND
	, HK_PHONE
	, HK_ISO
	, HK_CONFIG
	, HK_HOMEPAGE
	, HK_REFRESH
	, HK_EXIT
	, HK_MOVE
	, HK_EDIT
	, HK_SCROLLUP
	, HK_SCROLLDOWN
	, HK_KPLEFTPAREN
	, HK_KPRIGHTPAREN
	, HK_NEW
	, HK_REDO

	, HK_F13
	, HK_F14
	, HK_F15
	, HK_F16
	, HK_F17
	, HK_F18
	, HK_F19
	, HK_F20
	, HK_F21
	, HK_F22
	, HK_F23
	, HK_F24

	, HK_PLAYCD
	, HK_PAUSECD
	, HK_PROG3
	, HK_PROG4
	, HK_DASHBOARD
	, HK_SUSPEND
	, HK_CLOSE
	, HK_PLAY
	, HK_FASTFORWARD
	, HK_BASSBOOST
	, HK_PRINT
	, HK_HP
	, HK_CAMERA
	, HK_SOUND
	, HK_QUESTION
	, HK_EMAIL
	, HK_CHAT
	, HK_SEARCH
	, HK_CONNECT
	, HK_FINANCE
	, HK_SPORT
	, HK_SHOP
	, HK_ALTERASE
	, HK_CANCEL
	, HK_BRIGHTNESSDOWN
	, HK_BRIGHTNESSUP
	, HK_MEDIA

	, HK_SWITCHVIDEOMODE
	, HK_KBDILLUMTOGGLE
	, HK_KBDILLUMDOWN
	, HK_KBDILLUMUP

	, HK_SEND
	, HK_REPLY
	, HK_FORWARDMAIL
	, HK_SAVE
	, HK_DOCUMENTS

	, HK_BATTERY

	, HK_BLUETOOTH
	, HK_WLAN
	, HK_UWB

	, HK_UNKNOWN

	, HK_VIDEO_NEXT
	, HK_VIDEO_PREV
	, HK_BRIGHTNESS_CYCLE
	, HK_BRIGHTNESS_ZERO
	, HK_DISPLAY_OFF

	, HK_WIMAX
	, HK_RFKILL

	, HK_MICMUTE

	, HK_BTN_MISC
	, HK_BTN_0
	, HK_BTN_1
	, HK_BTN_2
	, HK_BTN_3
	, HK_BTN_4
	, HK_BTN_5
	, HK_BTN_6
	, HK_BTN_7
	, HK_BTN_8
	, HK_BTN_9

	, HK_BTN_LEFT
	, HK_BTN_RIGHT
	, HK_BTN_MIDDLE
	, HK_BTN_SIDE
	, HK_BTN_EXTRA
	, HK_BTN_FORWARD
	, HK_BTN_BACK
	, HK_BTN_TASK

	, HK_BTN_TRIGGER
	, HK_BTN_THUMB
	, HK_BTN_THUMB2
	, HK_BTN_TOP
	, HK_BTN_TOP2
	, HK_BTN_PINKIE
	, HK_BTN_BASE
	, HK_BTN_BASE2
	, HK_BTN_BASE3
	, HK_BTN_BASE4
	, HK_BTN_BASE5
	, HK_BTN_BASE6
	, HK_BTN_DEAD

	, HK_BTN_GAMEPAD
	, HK_BTN_A
	, HK_BTN_B
	, HK_BTN_C
	, HK_BTN_X
	, HK_BTN_Y
	, HK_BTN_Z
	, HK_BTN_TL
	, HK_BTN_TR
	, HK_BTN_TL2
	, HK_BTN_TR2
	, HK_BTN_SELECT
	, HK_BTN_START
	, HK_BTN_MODE
	, HK_BTN_THUMBL
	, HK_BTN_THUMBR

	, HK_BTN_DIGI
	, HK_BTN_TOOL_PEN
	, HK_BTN_TOOL_RUBBER
	, HK_BTN_TOOL_BRUSH
	, HK_BTN_TOOL_PENCIL
	, HK_BTN_TOOL_AIRBRUSH
	, HK_BTN_TOOL_FINGER
	, HK_BTN_TOOL_MOUSE
	, HK_BTN_TOOL_LENS
	, HK_BTN_TOOL_QUINTTAP
	, HK_BTN_TOUCH
	, HK_BTN_STYLUS
	, HK_BTN_STYLUS2
	, HK_BTN_TOOL_DOUBLETAP
	, HK_BTN_TOOL_TRIPLETAP
	, HK_BTN_TOOL_QUADTAP

	, HK_BTN_WHEEL
	, HK_BTN_GEAR_DOWN
	, HK_BTN_GEAR_UP

	, HK_OK
	, HK_SELECT
	, HK_GOTO
	, HK_CLEAR
	, HK_POWER2
	, HK_OPTION
	, HK_INFO
	, HK_TIME
	, HK_VENDOR
	, HK_ARCHIVE
	, HK_PROGRAM
	, HK_CHANNEL
	, HK_FAVORITES
	, HK_EPG
	, HK_PVR
	, HK_MHP
	, HK_LANGUAGE
	, HK_TITLE
	, HK_SUBTITLE
	, HK_ANGLE
	, HK_ZOOM
	, HK_MODE
	, HK_KEYBOARD
	, HK_SCREEN
	, HK_PC
	, HK_TV
	, HK_TV2
	, HK_VCR
	, HK_VCR2
	, HK_SAT
	, HK_SAT2
	, HK_CD
	, HK_TAPE
	, HK_RADIO
	, HK_TUNER
	, HK_PLAYER
	, HK_TEXT
	, HK_DVD
	, HK_AUX
	, HK_MP3
	, HK_AUDIO
	, HK_VIDEO
	, HK_DIRECTORY
	, HK_LIST
	, HK_MEMO
	, HK_CALENDAR
	, HK_RED
	, HK_GREEN
	, HK_YELLOW
	, HK_BLUE
	, HK_CHANNELUP
	, HK_CHANNELDOWN
	, HK_FIRST
	, HK_LAST
	, HK_AB
	, HK_NEXT
	, HK_RESTART
	, HK_SLOW
	, HK_SHUFFLE
	, HK_BREAK
	, HK_PREVIOUS
	, HK_DIGITS
	, HK_TEEN
	, HK_TWEN
	, HK_VIDEOPHONE
	, HK_GAMES
	, HK_ZOOMIN
	, HK_ZOOMOUT
	, HK_ZOOMRESET
	, HK_WORDPROCESSOR
	, HK_EDITOR
	, HK_SPREADSHEET
	, HK_GRAPHICSEDITOR
	, HK_PRESENTATION
	, HK_DATABASE
	, HK_NEWS
	, HK_VOICEMAIL
	, HK_ADDRESSBOOK
	, HK_MESSENGER
	, HK_BRIGHTNESS_TOGGLE
	, HK_SPELLCHECK
	, HK_LOGOFF

	, HK_DOLLAR
	, HK_EURO

	, HK_FRAMEBACK
	, HK_FRAMEFORWARD
	, HK_CONTEXT_MENU
	, HK_MEDIA_REPEAT
	, HK_10CHANNELSUP
	, HK_10CHANNELSDOWN
	, HK_IMAGES

	, HK_DEL_EOL
	, HK_DEL_EOS
	, HK_INS_LINE
	, HK_DEL_LINE

	, HK_FN
	, HK_FN_ESC
	, HK_FN_F1
	, HK_FN_F2
	, HK_FN_F3
	, HK_FN_F4
	, HK_FN_F5
	, HK_FN_F6
	, HK_FN_F7
	, HK_FN_F8
	, HK_FN_F9
	, HK_FN_F10
	, HK_FN_F11
	, HK_FN_F12
	, HK_FN_1
	, HK_FN_2
	, HK_FN_D
	, HK_FN_E
	, HK_FN_F
	, HK_FN_S
	, HK_FN_B

	, HK_BRL_DOT1
	, HK_BRL_DOT2
	, HK_BRL_DOT3
	, HK_BRL_DOT4
	, HK_BRL_DOT5
	, HK_BRL_DOT6
	, HK_BRL_DOT7
	, HK_BRL_DOT8
	, HK_BRL_DOT9
	, HK_BRL_DOT10

	, HK_NUMERIC_0
	, HK_NUMERIC_1
	, HK_NUMERIC_2
	, HK_NUMERIC_3
	, HK_NUMERIC_4
	, HK_NUMERIC_5
	, HK_NUMERIC_6
	, HK_NUMERIC_7
	, HK_NUMERIC_8
	, HK_NUMERIC_9
	, HK_NUMERIC_STAR
	, HK_NUMERIC_POUND

	, HK_CAMERA_FOCUS
	, HK_WPS_BUTTON

	, HK_TOUCHPAD_TOGGLE
	, HK_TOUCHPAD_ON
	, HK_TOUCHPAD_OFF

	, HK_CAMERA_ZOOMIN
	, HK_CAMERA_ZOOMOUT
	, HK_CAMERA_UP
	, HK_CAMERA_DOWN
	, HK_CAMERA_LEFT
	, HK_CAMERA_RIGHT

	, HK_ATTENDANT_ON
	, HK_ATTENDANT_OFF
	, HK_ATTENDANT_TOGGLE
	, HK_LIGHTS_TOGGLE

	, HK_BTN_DPAD_UP
	, HK_BTN_DPAD_DOWN
	, HK_BTN_DPAD_LEFT
	, HK_BTN_DPAD_RIGHT

	, HK_ALS_TOGGLE

	, HK_BUTTONCONFIG
	, HK_TASKMANAGER
	, HK_JOURNAL
	, HK_CONTROLPANEL
	, HK_APPSELECT
	, HK_SCREENSAVER
	, HK_VOICECOMMAND

	, HK_BRIGHTNESS_MIN
	, HK_BRIGHTNESS_MAX

	, HK_KBDINPUTASSIST_PREV
	, HK_KBDINPUTASSIST_NEXT
	, HK_KBDINPUTASSIST_PREVGROUP
	, HK_KBDINPUTASSIST_NEXTGROUP
	, HK_KBDINPUTASSIST_ACCEPT
	, HK_KBDINPUTASSIST_CANCEL

	, HK_BTN_TRIGGER_HAPPY
	, HK_BTN_TRIGGER_HAPPY1
	, HK_BTN_TRIGGER_HAPPY2
	, HK_BTN_TRIGGER_HAPPY3
	, HK_BTN_TRIGGER_HAPPY4
	, HK_BTN_TRIGGER_HAPPY5
	, HK_BTN_TRIGGER_HAPPY6
	, HK_BTN_TRIGGER_HAPPY7
	, HK_BTN_TRIGGER_HAPPY8
	, HK_BTN_TRIGGER_HAPPY9
	, HK_BTN_TRIGGER_HAPPY10
	, HK_BTN_TRIGGER_HAPPY11
	, HK_BTN_TRIGGER_HAPPY12
	, HK_BTN_TRIGGER_HAPPY13
	, HK_BTN_TRIGGER_HAPPY14
	, HK_BTN_TRIGGER_HAPPY15
	, HK_BTN_TRIGGER_HAPPY16
	, HK_BTN_TRIGGER_HAPPY17
	, HK_BTN_TRIGGER_HAPPY18
	, HK_BTN_TRIGGER_HAPPY19
	, HK_BTN_TRIGGER_HAPPY20
	, HK_BTN_TRIGGER_HAPPY21
	, HK_BTN_TRIGGER_HAPPY22
	, HK_BTN_TRIGGER_HAPPY23
	, HK_BTN_TRIGGER_HAPPY24
	, HK_BTN_TRIGGER_HAPPY25
	, HK_BTN_TRIGGER_HAPPY26
	, HK_BTN_TRIGGER_HAPPY27
	, HK_BTN_TRIGGER_HAPPY28
	, HK_BTN_TRIGGER_HAPPY29
	, HK_BTN_TRIGGER_HAPPY30
	, HK_BTN_TRIGGER_HAPPY31
	, HK_BTN_TRIGGER_HAPPY32
	, HK_BTN_TRIGGER_HAPPY33
	, HK_BTN_TRIGGER_HAPPY34
	, HK_BTN_TRIGGER_HAPPY35
	, HK_BTN_TRIGGER_HAPPY36
	, HK_BTN_TRIGGER_HAPPY37
	, HK_BTN_TRIGGER_HAPPY38
	, HK_BTN_TRIGGER_HAPPY39
	, HK_BTN_TRIGGER_HAPPY40

	, HK_X_SCROLL_LEFT
	, HK_X_SCROLL_RIGHT
	};
	return s_oSet;
}
	
} // namespace HardwareKeys

} // namespace stmi