package jsrl;

import jsrl.LoadLibrary;

public class Version
{

	
	static public int MAJOR;
	static public int MINOR;
	static public int REVISION;
	
	static protected native void _init(); 
	
	public static void main(String[] args)
	{
		System.out.println("SRL Version: " + MAJOR + "." + MINOR + "." + REVISION);
	}
}
