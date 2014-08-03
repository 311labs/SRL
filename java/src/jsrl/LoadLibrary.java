package jsrl;

public class LoadLibrary
{
    static public boolean Is64Bit()
    {
		String sunDataModel = System.getProperty("sun.arch.data.model");
        if (sunDataModel != null)
        {
            return "64".equals(sunDataModel);
        }
        String javaVMName = System.getProperty("java.vm.name");
        return ((javaVMName != null) && (javaVMName.indexOf("64") >= 0));
    }
    
    static public void Load()
    {
    	try
    	{
    		String sunDataModel = System.getProperty("sun.arch.data.model");
    		if (LoadLibrary.Is64Bit()) 
    		{
				System.loadLibrary("jsrl64");
            }
            else
            {
			    System.loadLibrary("jsrl");
			}
    	}
    	catch(UnsatisfiedLinkError e)
    	{
    		System.out.println("Could not load native code for library: JSRL");
    		System.out.println(e.getMessage());
    		System.exit(1);
    	}
    }
}


