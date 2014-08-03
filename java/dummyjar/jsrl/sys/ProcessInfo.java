package jsrl.sys;

import jsrl.sys.ProcessList;
import jsrl.sys.ProcessInfo.CPU;
import jsrl.sys.ProcessInfo.Memory;

public class ProcessInfo
{
	public class CPU
	{
		protected float _load;
		protected float _user_load;
		protected float _kernel_load;
		protected long _user_time;
		protected long _kernel_time;
		protected int _cpu_number;	
		
		/** current average load */
		public float load()
		{
			return _load;
		}
		/** current user load */
		public float userLoad()
		{
			return _user_load;
		}
		/** current kernel load */
		public float kernelLoad()
		{
			return _kernel_load;
		}
		/** cpu number (0 default) */
		public float cpuNumber()
		{
			return _cpu_number;
		}
		/** returns the total kernel time in jiffies (1/100) second */
		public long kernelTime()
		{
			return _kernel_time;
		}
		/** returns the total user time in jiffies (1/100) second */
		public long userTime()
		{
			return _user_time;
		}
				
		public void _update(float load, float user_load, float kernel_load, int number, long kernel_time, long user_time)
		{
			_load = load;
			_user_load = user_load;
			_kernel_load = kernel_load;
			_cpu_number = number;
			_kernel_time =kernel_time;
			_user_time = user_time;
		}
	}
	
	public class Memory
	{
		int _size, _used, _faults;
		/** total memory allocated to process */
		public int size()
		{
			return _size;
		}
		/** total memory used by process */
		public int used()
		{
			return _used;
		}
		/** number of page faults */
		public int faults()
		{
			return _faults;
		}
	}
	
	protected int _id = 0;
	protected String _name;
	protected String _filename;
	protected String _command;
	protected String _owner;
	protected int _parent_id;
	protected CPU _cpu;
	protected Memory _memory;
	protected int _threads;
	protected int _handles;
	protected int _priority;
	protected boolean _is_new;
	protected ProcessList _proc_list;

	
	public ProcessInfo(ProcessList proc_list, int id, int parent_id, String name, String owner)
	{
		_proc_list = proc_list;
		_id = id;
		_parent_id = parent_id;
		_name = name;
		_owner = owner;
		_cpu = new CPU();
		_memory = new Memory();
		_is_new = true;
	}
	
	/** terminates the process */
	public boolean terminate()
	{
		return ProcessList.KillProcess(_id);
	}
	
	/** check if the process is new */
	boolean isNew()
	{
		return _is_new;
	}
	
	/** called by native library for each process found during a refresh */
	void _update(int threads, int handles, int priority, 
			float load, float kernel_load, float user_load, int cpuNumber,
			int size, int used, int faults, long kernelTime, long userTime)
	{
		// this method is only called if the process already exists
		_threads = threads;
		_handles = handles;
		_priority = priority;
		_cpu._update(load, kernel_load, user_load, cpuNumber, kernelTime, userTime);
		_memory._size = size;
		_memory._used = used;
		_memory._faults = faults;
		_is_new = false;
	}
	
	/** update the current process information (normaly done by processlist) */
	public void refresh()
	{
		_proc_list._updateProc(_id);
	}
	
	/** the current process id */
	public int id()
	{
		return _id;
	}
	/** the process name */
	public String name()
	{
		return _name;
	}
	/** coming soon... */
	public String filename()
	{
		return _filename;
	}
	/** coming soon... */
	public String command()
	{
		return _command;
	}
	/** the current process owner */
	public String owner()
	{
		return _owner;
	}
	/** returns the parent id */
	public int parentId()
	{
		return _parent_id;
	}
	
	/** returns the parent process info */
	public ProcessInfo parent()
	{
		return _proc_list.getProcess(_parent_id);
	}
	
	/** current process cpu info */
	public CPU cpu()
	{
		return _cpu;
	}
	/** current process memory info */
	public Memory memory()
	{
		return _memory;
	}
	/** returns the thread count */
	public int threads()
	{
		return _threads;
	}
	/** returns the handle count */
	public int handles()
	{
		return _handles;
	}
	/** returns the handle count */
	public int priority()
	{
		return _priority;
	}
}
