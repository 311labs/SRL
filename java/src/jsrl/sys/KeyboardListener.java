package jsrl.sys;

import javax.swing.KeyStroke;

public interface KeyboardListener
{
	public void hotkey_pressed(int id, KeyStroke keystroke);
	public void hotkey_released(int id, KeyStroke keystroke);
}
