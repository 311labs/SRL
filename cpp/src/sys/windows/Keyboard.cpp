#ifdef WIN32

#include "srl/sys/Keyboard.h"
#include "srl/util/Vector.h"
#include "srl/Console.h"

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <winuser.h>

using namespace SRL;
using namespace SRL::System;
using namespace SRL::Util;

#define WM_SRL (WM_USER+1)
#define KEY_PRESSED 0x8000

Vector<Keyboard::HotKey> g_hotkeys(16);
HHOOK g_hhook = NULL;
THREAD_ID g_tid = 0;
bool g_ignore = false;
enum SRL_KB_EVENTS
{
	SRL_KB_ADD_HOTKEY,
	SRL_KB_REMOVE_HOTKEY,
	SRL_KB_HOTKEY_DOWN,
	SRL_KB_HOTKEY_UP
};

Keyboard::Keyboard(bool start):
_listener(NULL), _ids(100)
{
	if (start)
	{
		Thread::start();
		System::Sleep(100);
	}
}

Keyboard::~Keyboard()
{
	
}

uint32 Keyboard::registerHotkey(int modifier, int keycode, bool passthrough)
{
	Console::formatLine("jni: m=%d k=%d", modifier, keycode); 
	++_ids;
	HotKey hkey(_ids, modifier, keycode, passthrough, 1);
	::PostThreadMessage(Thread::id(), WM_SRL, SRL_KB_ADD_HOTKEY,  (LPARAM)&hkey);
	// now wait until our hotkey structure action is set to -1
	int counter = 0;
	while (hkey.action == 1)
	{	
		++counter;
		if (counter > 10)
			throw SRL::Errors::IOException("failed to register hot-key");
		System::Sleep(200);
	}
	return hkey.id;
}


void Keyboard::unregisterHotkey(const uint32& id)
{
	HotKey hkey(id, 0, 0, false, 2);
	::PostThreadMessage(Thread::id(), WM_SRL, SRL_KB_REMOVE_HOTKEY,  (LPARAM)&hkey);
	// now wait until our hotkey structure action is set to -1
	int counter = 0;
	while (hkey.action == 2)
	{	
		++counter;
		if (counter > 10)
			throw SRL::Errors::IOException("failed to unregister hot-key");
		System::Sleep(200);
	}
}

LRESULT CALLBACK HookProc(int code, WPARAM wparam, LPARAM lparam)
{
	if (g_ignore)
		return ::CallNextHookEx(g_hhook, code, wparam, lparam);

	KBDLLHOOKSTRUCT *kbevent = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);
	bool keydown = false;
	if ((wparam == WM_KEYDOWN) || (wparam == WM_SYSKEYDOWN))
		keydown = true;

	//Console::formatLine("key event: %x %d", wparam, kbevent->vkCode);
	// check if one of the modifier keys is pressed, possible hotkey
	// To Lock or Not to Lock... locking here would be pretty crazy for all the keyevents
	// to get around this we use the global ignore (g_ignore) when adding or removing
	// a new hotkey pair... and we actually delay for 500ms to ensure nothing is processing here
	for (int i=0; i < g_hotkeys.size(); ++i)
	{
		// check the state of active hotkeys during a key release
		if ((g_hotkeys[i].active) && (keydown == false))
		{
			int modifiers = 0;
			bool fire_release = false;
			// now we check modifiers
			if (g_hotkeys[i].modifiers > 0)
			{
				if ((g_hotkeys[i].modifiers & Keyboard::MODKEY_CTL) && (!(::GetAsyncKeyState(VK_CONTROL) & 0x80)))
					fire_release = true;
				if ((!fire_release)&&(g_hotkeys[i].modifiers & Keyboard::MODKEY_SHIFT) && (!(::GetAsyncKeyState(VK_SHIFT) & 0x80)))
					fire_release = true;
				if ((!fire_release)&&(g_hotkeys[i].modifiers & Keyboard::MODKEY_ALT) && (!(kbevent->flags & LLKHF_ALTDOWN)))
					fire_release = true;
			}
			else
				fire_release = true;

			if (fire_release)
			{
				::PostThreadMessage(g_tid, WM_SRL, SRL_KB_HOTKEY_UP, (LPARAM)&g_hotkeys[i]);
				g_hotkeys[i].active = false;
				if (g_hotkeys[i].passthrough)
					return CallNextHookEx(g_hhook, code, wparam, lparam);
				else
					return 1;
			}


		}
		else if ((keydown) && (g_hotkeys[i].keycode == kbevent->vkCode))
		{
			// check if the key is active then check passthrough
			if (g_hotkeys[i].active)
			{
				if (g_hotkeys[i].passthrough)
					return CallNextHookEx(g_hhook, code, wparam, lparam);
				else
					return 1;				
			}

			// if the hotkey is already active then this is either a repeat or the release event
			int modifiers = 0;
			// now we check modifiers
			if (::GetAsyncKeyState(VK_CONTROL) & KEY_PRESSED)
				modifiers += Keyboard::MODKEY_CTL;
			if (::GetAsyncKeyState(VK_SHIFT) & KEY_PRESSED)
				modifiers += Keyboard::MODKEY_SHIFT;
			if (kbevent->flags & LLKHF_ALTDOWN)
				modifiers += Keyboard::MODKEY_ALT;
			// now check if the modifiers are set
			if (g_hotkeys[i].modifiers == modifiers)
			{
				g_hotkeys[i].active = true;
				::PostThreadMessage(g_tid, WM_SRL, SRL_KB_HOTKEY_DOWN, (LPARAM)&g_hotkeys[i]);
				if (g_hotkeys[i].passthrough)
					return CallNextHookEx(g_hhook, code, wparam, lparam);
				else
					return 1;
			}
		}
	}
	
	return CallNextHookEx(g_hhook, code, wparam, lparam); 
}

void Keyboard::_hotkey_pressed(HotKey *hk)
{
    if (_listener != NULL)
		_listener->hotkey_event_down(hk->id);
}

void Keyboard::_hotkey_released(HotKey *hk)
{
    if (_listener != NULL)
		_listener->hotkey_event_up(hk->id);   
}

bool Keyboard::initial()
{
	// create keyboard hook
	g_hhook = ::SetWindowsHookEx(WH_KEYBOARD_LL, HookProc, SRL::GetDllInstance(), 0);
	g_tid = Thread::id();
    return true;
}

bool Keyboard::run()
{
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		if (msg.message == WM_SRL)
		{
			HotKey *hkey = (HotKey*)msg.lParam;
			switch (msg.wParam)
			{
				case SRL_KB_ADD_HOTKEY:
					// OUR "WORKAROUND" to not locking on every keyevent because it is so damn costly
					// ok first we turn on global ignore and wait 200ms to hopefully remove anyone from HookProc
					// !! actually no locking is needed because everything including HookProc is in this threads context
					// during the GetMessage call HookProc is called on keyboard events!!
					g_ignore = true;
					//System::Sleep(200);
					// now we are ready to add it to our queue, this just creates a copy
					g_hotkeys.add(*hkey);
					hkey->action = -1;
					g_ignore = false;
					break;
				case SRL_KB_REMOVE_HOTKEY:
					// OUR "WORKAROUND" to not locking on every keyevent because it is so damn costly
					// ok first we turn on global ignore and wait 200ms to hopefully remove anyone from HookProc
					g_ignore = true;
					//System::Sleep(200);
					// now we are ready to add it to our queue, this just creates a copy
					g_hotkeys.remove(*hkey);
					hkey->action = -1;
					g_ignore = false;
					break;
				case SRL_KB_HOTKEY_UP:
                    _hotkey_released(hkey);
					break;

				case SRL_KB_HOTKEY_DOWN:
					_hotkey_pressed(hkey);
					break;
			}
		}
		else
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}
	return true;
}

void Keyboard::final()
{
	// unregister the shell hook
	UnhookWindowsHookEx(g_hhook);
}


#endif // WIN32


