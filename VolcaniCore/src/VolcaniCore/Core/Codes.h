#pragma once
namespace VolcaniCore {

using KeyCode = unsigned short; // uint16_t

enum Key : KeyCode {
	// From glfw3.h starting line 394

	// Letter keys
	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,

	// Symbol keys
	Apostrophe   = 39, /* ' */
	Comma        = 44, /* , */
	Minus        = 45, /* - */
	Period       = 46, /* . */
	ForwardSlash = 47, /* / */
	Semicolon    = 59, /* ; */
	Equals       = 61, /* = */
	LeftBracket  = 91, /* [ */
	Backslash    = 92, /* \ */
	RightBracket = 93, /* ] */
	Backtick     = 96, /* ` */

	// Number keys
	Key0 = 48, /* 0 */
	Key1 = 49, /* 1 */
	Key2 = 50, /* 2 */
	Key3 = 51, /* 3 */
	Key4 = 52, /* 4 */
	Key5 = 53, /* 5 */
	Key6 = 54, /* 6 */
	Key7 = 55, /* 7 */
	Key8 = 56, /* 8 */
	Key9 = 57, /* 9 */

	// Function keys
	F1  = 290,
	F2  = 291,
	F3  = 292,
	F4  = 293,
	F5  = 294,
	F6  = 295,
	F7  = 296,
	F8  = 297,
	F9  = 298,
	F10 = 299,
	F11 = 300,
	F12 = 301,
	F13 = 302,
	F14 = 303,
	F15 = 304,
	F16 = 305,
	F17 = 306,
	F18 = 307,
	F19 = 308,
	F20 = 309,
	F21 = 310,
	F22 = 311,
	F23 = 312,
	F24 = 313,
	F25 = 314,

	Space       = 32,
	Escape      = 256,
	Return      = 257,
	Tab         = 258,
	Backspace   = 259,
	Insert      = 260,
	Delete      = 261,
	PageUp      = 266,
	PageDown    = 267,
	Home        = 268,
	End         = 269,
	CapsLock    = 280,
	ScrollLock  = 281,
	NumLock     = 282,
	PrintScreen = 283,
	Pause       = 284,

	LeftCtrl   = 224,
	LeftShift  = 225,
	LeftAlt    = 226,
	WindowsKey = 227,
	RightCtrl  = 228,
	RightShift = 229,
	RightAlt   = 230,
	
	Mute       = 127,
	VolumeUp   = 128,
	VolumeDown = 129,

	// Directional keys
	Right = 262,
	Left  = 263,
	Down  = 264,
	Up    = 265,
	
	// Keypad
	KP_1 = 320,
	KP_2 = 321,
	KP_3 = 322,
	KP_4 = 323,
	KP_5 = 324,
	KP_6 = 325,
	KP_7 = 326,
	KP_8 = 327,
	KP_9 = 328,
	KP_0 = 329,

	KP_Period   = 330,
	KP_Divide   = 331,
	KP_Multiply = 332,
	KP_Minus    = 333,
	KP_Plus     = 334,
	KP_Enter    = 335,

	// General case between two buttons
	Ctrl  = LeftCtrl + RightCtrl,
	Shift = LeftShift + RightShift,
	Alt   = LeftAlt + RightAlt,
};

using MouseCode = uint8_t; // uint8_t

enum Mouse : MouseCode {
	// From glfw3.h starting line 571

	Button1 = 0,
	Button2 = 1,
	Button3 = 2,
	Button4 = 3,
	Button5 = 4,
	Button6 = 5,
	Button7 = 6,
	Button8 = 7,

	LeftButton   = Button1,
	MiddleButton = Button2,
	RightButton  = Button3
};

enum class CursorMode : uint8_t
{
	Normal = 0,
	Hidden = 1,
	Locked = 2
};

}