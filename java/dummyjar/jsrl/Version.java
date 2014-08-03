package jsrl;

public class Version
{
	static
	{
		_init();

	}
	
	static public int MAJOR;
	static public int MINOR;
	static public int REVISION;
	
	static protected void _init()
	{
		MAJOR = 0;
		MINOR = 0;
		REVISION = 0;	 
	}
	
	public static void main(String[] args)
	{
		System.out.println("SRL Version: " + MAJOR + "." + MINOR + "." + REVISION);
	}
}
