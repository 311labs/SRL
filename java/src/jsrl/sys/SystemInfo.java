package jsrl.sys;

import java.util.Vector;

import jsrl.LoadLibrary;


public class SystemInfo
{
    static
    {
        LoadLibrary.Load();
    }
    
		
	protected String _hostname;
	protected String _domain;
	protected OS _os_info = new OS();
	protected Vector<CPU> _cpus = new Vector<CPU>();
	protected CPU _cpu_avg = new CPU();
	protected Memory _memory = new Memory();
	protected Memory _swap = new Memory();
	protected boolean _ht_supported = false;
	protected boolean _ht_enabled = false;
    protected int _physical_cpus = 0;
    protected int _logical_cpus = 0;
    protected int _cores_per_cpu = 0;
    
	
	private static SystemInfo __sysinfo_singleton = new SystemInfo();

	/** returns an instance of the Java Ping Utility */
	public static SystemInfo getInstance()
	{
		return __sysinfo_singleton;
	}
	
	public class CPU
	{
		public int _number;
		public float _speed;
		public String _vendorId;
		public String _model_name;
		public int _model, _family, _type;
		public int _cache;
		public float _load;
		public float _kernelLoad;
		public float _userLoad;
		public long _userTime;
		public long _kernelTime;
		public long _idleTime;
		
		public String vendor()
		{
			return _vendorId;
		}
		
		public int cache()
		{
			return _cache;
		}
		public float kernelLoad()
		{
			return _kernelLoad;
		}
		public float load()
		{
			return _load;
		}
		
		public int model()
		{
			return _model;
		}
		public int family()
		{
			return _family;
		}
		public int type()
		{
			return _type;
		}
		
		public String modelName()
		{
			return _model_name;
		}
		public int number()
		{
			return _number;
		}
		public float speed()
		{
			return _speed;
		}
		public float userLoad()
		{
			return _userLoad;
		}
		public long userTime()
		{
			return _userTime;
		}
		public long kernelTime()
		{
			return _kernelTime;
		}
		public long idleTime()
		{
			return _idleTime;
		}
	}
	
	public class OS
	{
		public String _name;
		public String _version;
		public String _kernel;
		public String _machine;

		public String name()
		{
			return _name;
		}
		
		public String version()
		{
			return _version;
		}
		
		public String kernel()
		{
			return _kernel;
		}
		
		public String machine()
		{
			return _machine;
		}
	}
	
	public class Memory
	{
		public int _total;
		public int _free;
		public int _used;
		
		public int total()
		{
			return _total;
		}
		
		public int free()
		{
			return _free;
		}
		
		public int used()
		{
			return _used;
		}
	}
	
	public SystemInfo()
	{
		// figure out the number of cpus
		int cpu_count = _getCpuCount();

		for (int i=0; i < cpu_count; ++i)
		{
			CPU cpu = new CPU();
			_cpus.add(cpu);
			// update each cpu
			_updateCpu(i, cpu, true);
		}
		// fill in the missing pieces that don't get updated
		CPU cpu = _cpus.get(0);
		_cpu_avg._vendorId = cpu._vendorId;
		_cpu_avg._model_name = cpu._model_name;
		_cpu_avg._model = cpu._model;
		_cpu_avg._type = cpu._type;
		_cpu_avg._family = cpu._family;
		// update the cpu average load info
		_updateAvg(_cpu_avg);
		// update the memory
		_updateMemory(_memory, _swap);
		// update host, user, and domain info
		_update(this, _os_info);
	}
	
	/** get the current hostname */
	public String hostname()
	{
		return _hostname;
	}
	
	/** get the current hostname */
	public String domain()
	{
		return _hostname;
	}
	
	/** get operating system info object */
	public OS os()
	{
		return _os_info;
	}
	
	/** get the cpu object that represents the average cpu info */
	public CPU cpu()
	{
		return _cpu_avg;
	}
	
	/** returns a specific cpu */
	public CPU getCPU(int index)
	{
		return _cpus.get(index);
	}
	
	/** get the current system memory information */
	public Memory memory()
	{
		return _memory;
	}
	
	/** get the current system swap information */
	public Memory swap()
	{
		return _swap;
	}
	
	/** get the current number of cpus */
	public int cpuCount()
	{
		return _cpus.size();
	}
	
	/** get the numnber of supported logical cpus */
	public int numberOfLogicalProcessors()
	{
        return _logical_cpus;
	}

	/** get the numnber of supported physical cpus */
	public int numberOfPhysicalProcessors()
	{
	    return _physical_cpus;
	}

	/** get the numnber of cores per cpu */
	public int numberOfCoresPerProcessor()
	{
	    return _cores_per_cpu;
	}
	
	/** is hyperthreading supported */
	public boolean isHtSupported()
	{
	    return _ht_supported;
	}
	
	/** is hyperthreading supported */
	public boolean isHtEnabled()
	{
	    return _ht_enabled;
	}

	/** updates the system info by calling the SRL Library */
	public void update()
	{
		// lazy hack to refresh sysinfo
		_getCpuCount();
		
		// check if this is the first time we are called
		for (int i=0; i < _cpus.size(); ++i)
		{
			_updateCpu(i, _cpus.get(i), false);
		}
		_updateAvg(_cpu_avg);
		// update the memory
		_updateMemory(_memory, _swap);
	}
	
	private native int _getCpuCount();
	private native void _updateCpu(int index, CPU cpu, boolean all);
	private native void _updateMemory(Memory memory, Memory swap);
	private native void _update(SystemInfo sysinfo, OS os);
	private native void _updateAvg(CPU avg);

	static public boolean Sleep(int milseconds)
	{
		
		try
		{
			Thread.sleep(milseconds);
		}
		catch (InterruptedException e)
		{
			return false;
		}
		return true;
	}	
	
	public static void main(String[] args) throws InterruptedException
	{
		SystemInfo sysinfo = SystemInfo.getInstance();
		System.out.println("OS: " + sysinfo.os().name());
		System.out.println("Version: " + sysinfo.os().version());
		System.out.println("Kernel: " + sysinfo.os().kernel());
		System.out.println("Hostname: " + sysinfo.hostname());
		System.out.println("Domain: " + sysinfo.domain());
		System.out.println("CPU Count: " + sysinfo.cpuCount());
		System.out.println("CPU Model: " + sysinfo.cpu().modelName());
		System.out.println("CPU Vendor: " + sysinfo.cpu().vendor());
		System.out.println("CPU Speed: " + sysinfo.cpu().speed());
		System.out.println("CPU HT Support: " + sysinfo.isHtSupported());
		System.out.println("CPU HT Enabled: " + sysinfo.isHtEnabled());
		System.out.println("CPU Physical CPUs: " + sysinfo.numberOfPhysicalProcessors());
		System.out.println("CPU Logical CPUs: " + sysinfo.numberOfLogicalProcessors());
		System.out.println("CPU Cores per CPU: " + sysinfo.numberOfCoresPerProcessor());
		
		while (true)
		{
			// wait atleast 1 second for cpu load information
			Sleep(1000);
			sysinfo.update();
			System.out.println("CPU Load: " + sysinfo.cpu().load());
			System.out.println("CPU User Load: " + sysinfo.cpu().userLoad());
			System.out.println("CPU Kernel Load: " + sysinfo.cpu().kernelLoad());
			System.out.println("Memory Total: " + sysinfo.memory().total());
			System.out.println("Memory Used: " + sysinfo.memory().used());
			System.out.println("Memory Free: " + sysinfo.memory().free());
			System.out.println("Swap Total: " + sysinfo.swap().total());
			System.out.println("Swap Used: " + sysinfo.swap().used());
			System.out.println("Swap Free: " + sysinfo.swap().free());
		}

	}
	
}
