/*
 * Created on Apr 28, 2005
 */
package jsrl.net;

import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.InetAddress;
import java.net.UnknownHostException;

import jsrl.LoadLibrary;

/**
 * JavaPing
 * Java Bindings for the SRL Library's Ping Logic
 */
public class JavaPing implements Runnable, PingListener
{
	public static final int ECHO_REPLY = 0;
	public static final int DESTINATION_UNREACHABLE = 3;

	// local listener
	protected PingListener _listener = null;
	protected boolean _is_running = false;
	private static JavaPing __javaping_singleton = null;
	protected boolean _has_rawsockets = false;
	
	
	static
	{
        LoadLibrary.Load();
        __javaping_singleton = new JavaPing();
	}

	/** returns an instance of the Java Ping Utility 
	 * @throws SocketException */
	public static JavaPing getInstance()
	{
        return __javaping_singleton;
	}

	private JavaPing()
	{
		_has_rawsockets = begin();
	}	
	
	public synchronized void startListening(PingListener listener) throws SocketException
	{
	    if (!_has_rawsockets)
	        throw new SocketException("permission denied attempting to create raw socket!");
		_listener = listener;
		new Thread(this).start();
	}

	public synchronized void stopListening()
	{
		_is_running = false;
	}

	public boolean isListening()
	{
		return _is_running;
	}
	
	protected int counter = 0;
	public int receivedCount()
	{
		return counter;
	}
	
	/** main method for performing reads */
	public void run()
	{
		counter = 0;
		_is_running = true;
		while (_is_running)
		{
			try
			{
				_listener.ping_result(pong(100));
				++counter;
			}
			catch (SocketTimeoutException e)
			{
			    
			}
			catch (SocketException e)
			{
				
			}
		}
		_listener = null;
	}
	
	static final PingResult DUMMY_RESULT = new PingResult();
	
	public void clearIncoming()
	{
		_pong(0, DUMMY_RESULT);
	}

	public PingResult pingpong(String address, int seq, int msg_size, int time_out) throws SocketTimeoutException
	{
	    return pingpong(address, seq, msg_size, time_out, 200);
	}
	    
	public PingResult pingpong(String address, int seq, int msg_size, int time_out, int ttl) throws SocketTimeoutException
	{
		PingResult result = new PingResult();
		if (!_pingpong(address, seq, msg_size, time_out, ttl, result))
		{
			throw new java.net.SocketTimeoutException("receiving echo reply timed out after "
					+ time_out);
		}
		return result;
	}
	
	public synchronized void ping(String address, int seq, int msg_size)
	{	    
	    ping(address, seq, msg_size, 200);
	}
	
	/** send a ICMP Echo Request to the specified ip */
	public native void ping(String address, int seq, int msg_size, int ttl);

	/**pthread
	 * recv a ICMP Echo Reply
	 * @param time_out in milliseconds
	 * @throws SocketTimeoutException
	 */
	public PingResult pong(int time_out) throws SocketException, SocketTimeoutException
	{
	    if (!_has_rawsockets)
	        throw new SocketException("permission denied attempting to create raw socket!");
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
	private native boolean begin();
	/** native call for ping reply */
	private native boolean _pong(int timeout, PingResult result);
	/** native call that will use ICMP.dll on windows */
	private native boolean _pingpong(String address, int seq, int msg_size, int timeout, int ttl, PingResult result);
	/** free pinger resources */
	public native void end();


	public void ping_result(PingResult result)
	{
		if (result.type == ECHO_REPLY)
		{
			System.out.println("Reply from: " + result.from + "  icmp_seq=" + result.seq + "  bytes: " + result.bytes + "   time=" + result.rtt + "  TTL=" + result.ttl);
		}
		else if (result.type == DESTINATION_UNREACHABLE)
		{
			System.out.println("DESTINATION UNREACHABLE icmp_seq=" + result.seq);
		}
		else
		{
			System.out.println("unknown icmp packet received");
		}
	}


	public static void main(String[] args) throws InterruptedException, UnknownHostException, SocketTimeoutException
	{

		if (args.length == 0)
		{
			System.out.println("usage: ping host [options]");
			return;
		}

		String address = "127.0.0.1";
		int count = 5;
		int size = 56;
		int delay = 1000;
		for (int i = 0; i < args.length; ++i)
		{
			if (args[i].compareToIgnoreCase("-c") == 0)
			{
				++i;
				if (i < args.length) count = Integer.parseInt(args[i]);
			}
			else if (args[i].compareToIgnoreCase("-s") == 0)
			{
				++i;
				if (i < args.length) size = Integer.parseInt(args[i]);
			}
			else if (args[i].compareToIgnoreCase("-d") == 0)
			{
				++i;
				if (i < args.length) delay = Integer.parseInt(args[i]);
			}
			else
			{
				address = args[i];
			}
		}
		System.out.println("count: " + count + " size: " + size + "  delay: " + delay);
		
		address = InetAddress.getByName(address).getHostAddress();
		JavaPing pingman;
		pingman = getInstance();
		//pingman.startListening(pingman);
		//Thread.sleep(100);
		for (int i = 0; i < count; ++i)
		{
			pingman.ping_result(pingman.pingpong(address, i, size, 500));
			Thread.sleep(delay);
		}
		
		if (pingman.receivedCount() < count)
			System.out.println("DID NOT RECEIVE REPLIES FOR SOME PACKETS!");

		pingman.end();


	}
}
