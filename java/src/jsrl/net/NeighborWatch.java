/*
 * Version: $Revision: 97 $
 * Last Author:  $Author: ians $    Last Modified:  $Date: 2005-09-09 00:52:30 -0400 (Fri, 09 Sep 2005) $
 * Created on May 10, 2005 by ians
 */

package jsrl.net;

import java.net.SocketException;
import java.util.List;
import java.util.Vector;

import jsrl.net.JavaPing;
import jsrl.net.PingListener;
import jsrl.net.PingResult;

/**
 * @author ians
 *
 * Watches a list of ips for availability
 * 
 * TODO make the update rate variable
 * TODO make smarter notification.. only notify if the old list does not match the new list
 */
public class NeighborWatch implements Runnable, PingListener
{
	/** local storage for watch list */
	protected String[] _watch_list = null;
	/** listener to send change event to */
	protected NeighborWatchListener _listener = null;
	/** Native Java Ping Tool (Must be root on unix) */
	protected JavaPing _ping = null;
	/** thread state */
	protected boolean _is_running = false;
	/** list of available neighbors */
	protected List<String> _available = new Vector<String>();
	
	
	/** default constructor takes a watch list */
	public NeighborWatch(NeighborWatchListener listener)
	{
		_listener = listener; 
	}
	
	/** start watching the following list */
	public void start(String[] watch_list)
	{
		// yeah this is bad.. threading collision but it works for now
		_watch_list = watch_list;
		
		if (_is_running == false)
		{
			_is_running = true;
			// automatically spawn the wath thread
			new Thread(this).start();
		}
	}
	
	/** stop watching */
	public void stop()
	{
		_is_running = false;
	}
	
	/** returns a list of currently available neighbors */
	public List getAvailable()
	{
		return null;
	}
	
	/** main method for performing watch */
	public void run()
	{
		try
		{
			_ping = JavaPing.getInstance();
			_ping.startListening(this);
		}
		catch (SocketException e1)
		{
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		while (_is_running)
		{
			// clear the existing available list
			synchronized(_available)
			{
				_available.clear();
			}
			
			// ping each address in our watch list
			for (int i=0; i < _watch_list.length; ++i)
			{
				_ping.ping(_watch_list[i], i, 16);
			}
			
			try 
			{
				// now lets wait for 5 seconds for results
				// results will come in via the ping_result callback
				Thread.sleep(5000);
			} catch (InterruptedException e) 
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			// report the available list to our listener
			_listener.neighbor_change(_available);
			
		}
		_ping.stopListening();
		// clean up ping resources
		_ping.close();
	}

	/* (non-Javadoc)
	 * @see jsrl.net.PingListener#ping_result(jsrl.net.PingResult)
	 */
	public void ping_result(PingResult result) 
	{		
		synchronized(_available)
		{
			_available.add(result.from);
		}			
	}
}
