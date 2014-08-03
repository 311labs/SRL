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
		// now we need to inform the listener of all the existing processes
		for (ProcessInfo proc_info : _cache.values())
		{
			listener.new_process(proc_info);
		}
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
			
//			if (is_new)
//			{
//				// now tell everyone who is interested about the new process
//				for (ProcessListener listener : _listeners)
//				{
//					listener.new_process(proc);
//				}
//			}
		}
	}
	
	/** called by native library for each new process */
	void _new_process(int pid, int parent_id, String name, String owner, String filename, String command)
	{
		ProcessInfo proc = new ProcessInfo(this, pid, parent_id, name, owner, filename, command);
		_cache.put(pid, proc);
		// don't tell listeners about the new process until it is updated
		for (ProcessListener listener : _listeners)
		{
			listener.new_process(proc);
		}		
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
	public synchronized ProcessInfo getProcess(int pid)
	{
		/** check cache of procs */
		if (_cache.containsKey(pid))
			return _cache.get(pid);
		return null;
	}
	
	/** returns a ProcessInfo object by its name */
	public synchronized ProcessInfo getProcess(String name)
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
	static public native boolean KillProcess(int pid);
	
	/** native call to initialize process listening */
	private native void _init();
	/** native call to end process listening */
	public native void end();
	/** native call to update process list */
	private native void _update();
	/** native call that tells the jsrl lib to update the process with the passed in pid */
	public native boolean _updateProc(int pid);

	public static int _thread_loop = 0;
	public static int _thread_sleep = 0;

	public static void thrashTest(int thread_time)
	{
		int thread_count = 20;
		_thread_sleep = 50;
		_thread_loop = (thread_time*1000)/_thread_sleep;
		System.out.print("Starting Thrash Test\n");
		System.out.format("\tthreads: %d\n\ttest time: %d seconds\n\tthread sleep: %d\n", thread_count, thread_time, _thread_sleep);
		List<Thread> threads = new Vector<Thread>();
		for (int t=0; t<thread_count; ++t)
		{
			Thread th = new Thread(new Runnable() {
				public void run() {
					ProcessList proclist = getInstance();
					for (int i=0; i<_thread_loop; ++i)
					{
						proclist.update();
						ProcessInfo proc = proclist.getProcess("svchost.exe");
						if (proc == null)
							System.out.print("\tthread: could not find svchost.exe\n");
						try
						{
							Thread.sleep(_thread_sleep);
						} catch (InterruptedException e)
						{
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					System.out.print("\tTHREAD EXITED\n");
				}
			});
			
			th.start();
			threads.add(th);
		}
		
		int sleep_count = 0;
		int sleep_max = thread_time*1000;
		while (threads.size() > 0)
		{
			try
			{
				Thread.sleep(5000);
				sleep_count += 5000;
			} catch (InterruptedException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			if (sleep_count >= sleep_max)
			{
				Iterator<Thread> it=threads.iterator();
				while (it.hasNext())
				{
					Thread th = it.next();
					if (!th.isAlive())
					{
						it.remove();
					}
				}
				System.out.format("waiting on %d threads\n", threads.size());
			}
			else
			{
				System.out.format("%d seconds before thread exits\n", (sleep_max - sleep_count)/1000);
			}
		}
		threads.clear();
	}
	
	public static void main(String[] args)
	{
		ProcessList proclist = getInstance();
		SystemInfo sysinfo = SystemInfo.getInstance();
		//proclist.update();
		boolean flag = true;
		
		for (int i=0; i<args.length; ++i)
		{
			if (args[i].equalsIgnoreCase("-k"))
			{
				// now kill each pid or process
				for (int p=i+1; p<args.length; ++p)
				{
					ProcessInfo pinfo = proclist.getProcess(args[p]);
					if (pinfo != null)
					{
						pinfo.terminate();
					}
				}
				System.exit(0);
			}
			else if (args[i].equalsIgnoreCase("-t"))
			{
				thrashTest(Integer.valueOf(args[i+1]));
				System.exit(0);
			}
		}
		long old_time  = sysinfo.cpu().userTime() + sysinfo.cpu().kernelTime() + sysinfo.cpu().idleTime();
		long new_time  = 0;
		while (flag)
		{
			SystemInfo.Sleep(1000);
			proclist.update();
			sysinfo.update();
			Map<Integer, ProcessInfo> procmap = proclist.getList();

			float mem_perct = ((float)sysinfo.memory().used()/(float)sysinfo.memory().total())*100.0f;
			System.out.format("\n\nTotal Procs: %d	 CPU: %.2f%%  Memory: %.2f%% (used: %d MB)\n\n", procmap.size(),
					sysinfo.cpu().load(), mem_perct, sysinfo.memory().used()/1024);
			System.out.format("%-20s%-10s%-10s%-10s%-15s%-15s\n", "image", "pid", "parent", "cpu", "memory", "owner");

			new_time = 0;
			
			System.out.println("CPU Time:" + (sysinfo.cpu().userTime() + sysinfo.cpu().kernelTime() + sysinfo.cpu().idleTime()) );
			old_time = new_time;

			System.out.println("-------------------------------------------------------------------");
			for (ProcessInfo proc : procmap.values())
			{	
				if (proc.cpu().load() > 0.0f)
				{
					System.out.format("%-20s%-10d%-10s%-10.2f%-15d%-15s\n", 
							proc.name(), proc.id(), proc.parentId(), 
							proc.cpu().load(), proc.memory().size(), proc.owner());
							
				}
			}
		}
		proclist.end();
	}
	
}
