#include "srl/sys/Keyboard.h"
#ifndef WIN32

#include "srl/util/Vector.h"
#include "srl/Console.h"

using namespace SRL;
using namespace SRL::System;
using namespace SRL::Util;


Keyboard::Keyboard(bool start):
_hotkeys(), _ids(100), _listener(NULL)
{

}

Keyboard::~Keyboard()
{
	
}

uint32 Keyboard::registerHotkey(int modifier, int keycode, bool passthrough)
{
    return 0;
}


void Keyboard::unregisterHotkey(const uint32& id)
{
    
}

bool Keyboard::initial()
{
    return false;
}

bool Keyboard::run()
{
    return false;
}

void Keyboard::final()
{

}

void Keyboard::_hotkey_pressed(HotKey *hk)
{

}

void Keyboard::_hotkey_released(HotKey *hk)
{
  
}

#endif // OSX


