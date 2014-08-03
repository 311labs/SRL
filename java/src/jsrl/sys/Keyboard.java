package jsrl.sys;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import javax.swing.KeyStroke;
import jsrl.LoadLibrary;

public class Keyboard
{
    
    static
    {
        LoadLibrary.Load();
    }
    
	/** cache of active procs (they may be dead) */
	private Map<Integer, KeyStroke> _hotkeys = new HashMap<Integer, KeyStroke>();
	private List<KeyboardListener> _listeners = new Vector<KeyboardListener>();
	
	private static Keyboard __keyboard_singleton = null;

	/** returns an instance of the Java Ping Utility */
	public synchronized static Keyboard getInstance()
	{
		if (__keyboard_singleton == null)
		{
			__keyboard_singleton = new Keyboard();
			__keyboard_singleton.start();
		}
		return __keyboard_singleton;
	}	
	
	/** returns an instance of the Java Ping Utility */
	public synchronized static void freeInstance()
	{
		if (__keyboard_singleton != null)
		{
			__keyboard_singleton.stop();
			__keyboard_singleton = null;
		};
	}
	
	/** reset the keyboard hooks */
	public synchronized void resetHooks()
	{
		__keyboard_singleton.stop();		
		__keyboard_singleton.start();
	}
	
	private Keyboard()
	{

	}	
	
	/** add a keyboard listener */
	public void addListener(KeyboardListener listener)
	{
		_listeners.add(listener);
	}
	
	/** remove a keyboard listener */
	public void removeListener(KeyboardListener listener)
	{
		if (_listeners.contains(listener))
		{
			_listeners.remove(listener);
		}
	}
	
	public int registerHotKey(KeyStroke hotkey, boolean passthrough)
	{
		System.out.format("new hotkey: m=%d k=%d\n", hotkey.getModifiers(), hotkey.getKeyCode());
		int modifiers = hotkey.getModifiers();
		int new_mods = 0;
		if ((modifiers & java.awt.event.InputEvent.CTRL_MASK) != 0)
			new_mods += 2;
		if ((modifiers & java.awt.event.InputEvent.SHIFT_MASK) != 0)
			new_mods += 1;
		if ((modifiers & java.awt.event.InputEvent.ALT_MASK) != 0)
			new_mods += 8;
		if ((modifiers & java.awt.event.InputEvent.META_MASK) != 0)
			new_mods += 4;

	    int id = _registerHotKey(new_mods, hotkey.getKeyCode(), passthrough);
	    _hotkeys.put(id, hotkey);
	    return id;
	}
	
	public void unregisterHotKey(int id)
	{
	    // TODO throw exception if does not exists
	    if (!_hotkeys.containsKey(id))
	        return;
	    _unregisterHotKey(id);
	    _hotkeys.remove(id);
	}
	
	/** called from the native library when the keyevent happens */
	void _hotkey_pressed(int id)
	{
	    if (!_hotkeys.containsKey(id))
	        return;
	        
	    KeyStroke keystroke = _hotkeys.get(id);
		for (KeyboardListener listener : _listeners)
		{
			listener.hotkey_pressed(id, keystroke);
		}    
	}

	/** called from the native library when the keyevent happens */
	void _hotkey_released(int id)
	{
	    if (!_hotkeys.containsKey(id))
	        return;
	        
	    KeyStroke keystroke = _hotkeys.get(id);
		for (KeyboardListener listener : _listeners)
		{
			listener.hotkey_released(id, keystroke);
		}
	}

	/** native call to initialize process listening */
	protected native void start();
	/** native call to end process listening */
	protected native void stop();
	/** native call to register a hotkey */
	private native int _registerHotKey(int modifiers, int keycode, boolean passthrough);
	/** native call to unregister a hotkey */
	private native void _unregisterHotKey(int id);
	
	
	public static void main(String[] args) throws InterruptedException
	{
		class TestKeyboard implements KeyboardListener
		{
			public TestKeyboard()
			{
				
			}
			
			public void hotkey_pressed(int id, KeyStroke keystroke)
			{
				System.out.format("hotkey pressed %d\n", id);
				
			}

			public void hotkey_released(int id, KeyStroke keystroke)
			{
				System.out.format("hotkey released %d\n", id);
			}
			
		}	
		
		System.out.format("ctl=%d,", java.awt.event.InputEvent.CTRL_MASK);
		System.out.format("alt=%d,", java.awt.event.InputEvent.ALT_MASK);
		System.out.format("shift=%d,", java.awt.event.InputEvent.SHIFT_MASK);
		System.out.format("meta=%d\n", java.awt.event.InputEvent.META_MASK);
		
		TestKeyboard tkb = new TestKeyboard();
	    Keyboard kb = Keyboard.getInstance();
	    kb.addListener(tkb);
		int id = kb.registerHotKey(KeyStroke.getKeyStroke('A', java.awt.event.InputEvent.CTRL_MASK), false);
		
		
		
		for (int i=0; i<10; ++i)
		{
			Thread.sleep(1000);
		}
		
		kb.unregisterHotKey(id);
		
		kb.registerHotKey(KeyStroke.getKeyStroke('A', java.awt.event.InputEvent.CTRL_MASK), true);
		
		
	}
	
}