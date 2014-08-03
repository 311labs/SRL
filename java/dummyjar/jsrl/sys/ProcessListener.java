package jsrl.sys;

public interface ProcessListener
{
	public void new_process(ProcessInfo proc);
	public void dead_process(ProcessInfo proc);
}
