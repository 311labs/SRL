/*
 * Created on Apr 28, 2005
 */
package jsrl.net;

import java.net.SocketException;
import java.net.SocketTimeoutException;

/**
 * JavaPing
 * Java Bindings for the SRL Library's Ping Logic
 */
public class JavaPing implements Runnable
{
	// local listener
	protected PingListener _listener = null;
	protected boolean _is_running = false;
	private static JavaPing __javaping_singleton = null;
	

	/** returns an instance of the Java Ping Utility 
	 * @throws SocketException */
	public static JavaPing getInstance() throws SocketException
	{
		if (__javaping_singleton == null)
			throw new SocketException("permission denied attempting to create raw socket!");
		return __javaping_singleton;
	}

	private JavaPing() throws SocketException
	{
		if (!begin())
			throw new SocketException("permission denied attempting to create raw socket!");
	}	
	
	public synchronized void startListening(PingListener listener)
	{
		_listener = listener;
	}

	public synchronized void stopListening()
	{
		_is_running = false;
	}

	/** main method for performing reads */
	public void run()
	{
		while (_is_running)
		{
			try
			{
				_listener.ping_result(pong(100));
			}
			catch (SocketTimeoutException e)
			{
				
			}
		}
		_listener = null;
	}

	public PingResult pingpong(String address, int seq, int msg_size, int time_out) throws SocketTimeoutException
	{
			ping(address, seq, msg_size);
			return pong(time_out);
	}
	
	/** send a ICMP Echo Request to the specified ip */
	public void ping(String address, int seq, int msg_size){;}

	/**pthread
	 * recv a ICMP Echo Reply
	 * @param time_out in milliseconds
	 * @throws SocketTimeoutException
	 */
	public PingResult pong(int time_out) throws SocketTimeoutException
	{
		PingResult result = new PingResult();
		if (!_pong(time_out, result))
		{
			throw new java.net.SocketTimeoutException("receiving echo reply timeout after "
					+ time_out);
		}
		return result;
	}	
	
	/** closes the Java Ping utility free resources */
	public void close()
	{
		synchronized (JavaPing.class)
		{
			if (__javaping_singleton != null)
			{
				end();
				__javaping_singleton = null;
			}
		}		
	}
	
	/** initialize our cpp code */
	private boolean begin(){ return false;}
	/** native call for ping reply */
	private boolean _pong(int timeout, PingResult result){ return false;}
	/** free pinger resources */
	public void end(){}
	


	public static void main(String[] args) throws InterruptedException
	{

	}
}
