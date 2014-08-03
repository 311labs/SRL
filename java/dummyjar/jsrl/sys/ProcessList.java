package jsrl.sys;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;

public class ProcessList
{
	/** cache of active procs (they may be dead) */
	private Map<Integer, ProcessInfo> _cache = new HashMap<Integer, ProcessInfo>();
	private List<ProcessListener> _listeners = new Vector<ProcessListener>();
	private boolean _inited = false;
	
	private static ProcessList __proclist_singleton = new ProcessList();

	/** returns an instance of the Java Ping Utility */
	public static ProcessList getInstance()
	{
		return __proclist_singleton;
	}	
	
	
	private ProcessList()
	{

	}
	
	/** updates the process list, needs to be called atleast once */
	public synchronized void update()
	{
		if (!_inited)
		{
			_init();
			_inited = true;
			return;
		}
		_update();
	}
	
	/** add a process listener */
	public void addListener(ProcessListener listener)
	{
		_listeners.add(listener);
	}
	
	/** remove a process listener */
	public void removeListener(ProcessListener listener)
	{
		if (_listeners.contains(listener))
		{
			_listeners.remove(listener);
		}
	}
	
	/** get the current process map */
	public Map<Integer, ProcessInfo> getList()
	{
		return _cache;
	}	
	
	/** called by native library for each process found during a refresh 
	 * TODO REMOVE PUBLIC */
	void _update_process(int pid, int threads,int handles, int priority, 
			float load, float kernel_load, float user_load, int cpuNumber,
			int size, int used, int faults, long kernelTime, long userTime)
	{
		// this method is only called if the process already exists
		if (_cache.containsKey(pid))
		{
			ProcessInfo proc = _cache.get(pid);
			boolean is_new = proc.isNew();	
			proc._update(threads, handles, priority, load, kernel_load,
					user_load, cpuNumber, size, used, faults, kernelTime, userTime);
			
			if (is_new)
			{
				// now tell everyone who is interested about the new process
				for (ProcessListener listener : _listeners)
				{
					listener.new_process(proc);
				}
			}
		}
	}
	
	/** called by native library for each new process */
	void _new_process(int pid, int parent_id, String name, String owner)
	{
		ProcessInfo proc = new ProcessInfo(this, pid, parent_id, name, owner);
		_cache.put(pid, proc);
		// don't tell listeners about the new process until it is updated
	}
	
	/** called by native library for each dead process */
	void _dead_process(int pid)
	{
		// this method is only called if the process already exists
		if (_cache.containsKey(pid))
		{
			ProcessInfo proc = _cache.get(pid);
			// now tell everyone who is interested about the dead process
			for (ProcessListener listener : _listeners)
			{
				listener.dead_process(proc);
			}
			
			// now lets remove the process from our cache
			_cache.remove(pid);
			
		}
	}
	
	/** returns a ProcessInfo object by its pid */
	public ProcessInfo getProcess(int pid)
	{
		/** check cache of procs */
		if (_cache.containsKey(pid))
			return _cache.get(pid);
		return null;
	}
	
	/** returns a ProcessInfo object by its name */
	public ProcessInfo getProcess(String name)
	{
		/** check cache of procs */
		Iterator<Integer> iter = _cache.keySet().iterator();
		while (iter.hasNext())
		{
			int pid = iter.next();
			ProcessInfo proc = _cache.get(pid);
			if (proc.name().equalsIgnoreCase(name))
			{
				return proc;
			}
		}
		
		return null;
	}
	
	/** returns the number of processes in the list */
	public int size()
	{
		return _cache.size();
	}
	
	/** native call to kill a process */
	static public boolean KillProcess(int pid){ return false; }
	
	/** native call to initialize process listening */
	private void _init(){};
	/** native call to end process listening */
	public void end(){};
	/** native call to update process list */
	private void _update(){};
	/** native call that tells the jsrl lib to update the process with the passed in pid */
	public boolean _updateProc(int pid){ return false; }
	
	public static void main(String[] args)
	{

	}
	
}
