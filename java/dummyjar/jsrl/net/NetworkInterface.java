package jsrl.net;

import java.util.List;
import java.util.Vector;

import jsrl.sys.SystemInfo;

/** Network Interface Statistics */
public class NetworkInterface
{
	public enum StatView
	{
		TOTAL,
		SINCE_UPDATE,
		SINCE_CLEAR
	}
	

	protected String _name, _ip, _mac, _ap_mac, _essid;
	protected boolean _is_loopback, _is_wifi, _is_up;
	protected IoStats _rx = new IoStats();
	protected IoStats _tx = new IoStats();
	static protected List<NetworkInterface> __interfaces = null; 
	
	public void setView(StatView view)
	{
		_rx.setView(view);
		_tx.setView(view);
	}
	
	/** clear the current stats to start fresh */
	public void clear()
	{
		_rx.clear();
		_tx.clear();
	}
	
	/** update the current interface properties */
	public void update()
	{
		_update_iface(this);
	}
	
	/** returns the interface name */
	public String name()
	{
		return _name;
	}
	
	/** returns the ip address of the interface */
	public String ip()
	{
		return _ip;
	}
	
	/** returns the mac address of the interface */
	public String mac()
	{
		return _mac;
	}
	
	/** return the essid if interface is wireless */
	public String essid()
	{
		return _essid;
	}
	
	/** return the AccessPoint address */
	public String apMac()
	{
		return _ap_mac;
	}
	
	/** returns true if loopback device */
	public boolean isLoopback()
	{
		return _is_loopback;
	}
	
	/** returns true if wireless device */
	public boolean isWireless()
	{
		return _is_wifi;
	}
	
	/** returns true if device is up */
	public boolean isUp()
	{
		return _is_up;
	}
	
	/** returns the receive stats */
	public IoStats rx()
	{
		return _rx;
	}
	
	/** returns the transmit stats */
	public IoStats tx()
	{
		return _tx;
	}
	
	/** return string representation */
	public String toString()
	{
		String out = String.format("%-10s ip: %-16s mac: %-20s\n\trx: %s\ttx: %s",
				_name, _ip, _mac, _rx.toString(), _tx.toString());
		if (isWireless())
		{
			out += String.format("\tessid: %-20s ap mac: %-20s\n", _essid, _ap_mac);
		}
		return out + '\n';
	}
	
	protected NetworkInterface(String name)
	{
		_name = name;
		_update_iface(this);
	}
	
	/** update all internal interfaces */
	static synchronized public void updateList()
	{
		if (__interfaces == null)
		{
			__interfaces = new Vector<NetworkInterface>();
		}
		_update();
	}
	
	/** return a List of all the current interfaces */
	static public List<NetworkInterface> getAll()
	{
		if (__interfaces == null)
			updateList();	
		return __interfaces;
	}
	
	/** retrieve an interface by its name */
	static public NetworkInterface getByName(String name)
	{
		if (__interfaces == null)
			updateList();
		
		for (NetworkInterface iface : __interfaces)
		{
			if (iface.name().equalsIgnoreCase(name))
			{
				return iface;
			}
		}
		return null;
	}

	/** retrieve an interface by its name */
	static public NetworkInterface getByAddress(String ip)
	{
		if (__interfaces == null)
			updateList();
		
		for (NetworkInterface iface : __interfaces)
		{
			if (iface.ip().equalsIgnoreCase(ip))
			{
				return iface;
			}
		}
		return null;
	}
	
// INTERNAL NATIVE CALLBACKS
	static protected boolean _update(){ return false;}
	static protected boolean _update_iface(NetworkInterface iface){ return false;}
		

	static void _new_interface(String name)
	{
		NetworkInterface iface = getByName(name);
		if (iface == null)
		{
			iface = new NetworkInterface(name);
			__interfaces.add(iface);
		}
		else
		{
			_update_iface(iface);
		}
	}
	
	static void _dead_interface(String name)
	{
		NetworkInterface iface = getByName(name);
		if (iface != null)
		{
			__interfaces.remove(iface);
		}
	}
	
	
	void _update_rx(long bytes, long packets, long errors, long drops, long multicast)
	{
		_rx.update(bytes, packets, errors, drops, multicast);
	}
	
	void _update_tx(long bytes, long packets, long errors, long drops)
	{
		_tx.update(bytes, packets, errors, drops, 0);
	}
	/**
	 * @param args
	 */
	public static void main(String[] args)
	{

	}

}
