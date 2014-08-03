package jsrl.net;

import java.io.Serializable;

/** Network Interface Input Output stats */
public class IoStats implements Serializable
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public class IoValue implements Serializable
	{
		/**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		protected long total=0;
		protected long cleared_at=0;
		protected long last=0;
		protected long extra=0;
		protected NetworkInterface.StatView view=NetworkInterface.StatView.TOTAL;
		
		public NetworkInterface.StatView getView()
		{
			return view;
		}
		
		public void setView(NetworkInterface.StatView view)
		{
			this.view = view;
		}
		
		public long value()
		{
			switch (view)
			{
			case TOTAL:
				return total;
			case SINCE_UPDATE:
				return sinceUpdate();
			case SINCE_CLEAR:
				return sinceCleared();
			}
			return total;
		}
		
		public void clear()
		{
			cleared_at = total;
		}
		
		public void update(long current)
		{
			if (current >= total)
			{
				last = total;
			}
			else
			{
				extra += sinceCleared();
				last = 0;
				cleared_at = 0;
			}
			total = current;
		}
		
		public long total()
		{
			return total + extra;
		}
		
		public long sinceCleared()
		{
			return total - cleared_at + extra;
		}
		
		public long sinceUpdate()
		{
			return total - last;
		}

	}
	
	protected IoValue _bytes = new IoValue();
	protected IoValue _packets = new IoValue();
	protected IoValue _drops = new IoValue();
	protected IoValue _errors = new IoValue();
	protected IoValue _multicast = new IoValue();
	
	public IoStats()
	{

	}
	
	public void setView(NetworkInterface.StatView view)
	{
		_bytes.setView(view);
		_packets.setView(view);
		_drops.setView(view);
		_errors.setView(view);
		_multicast.setView(view);
	}
	
	public NetworkInterface.StatView getView()
	{
		return _bytes.getView();
	}
	
	public long bytes()
	{
		return _bytes.value();
	}

	public long packets()
	{
		return _packets.value();
	}
	
	public long multicast()
	{
		return _multicast.value();
	}
	
	public long errors()
	{
		return _errors.value();
	}
	
	public long drops()
	{
		return _drops.value();
	}
	
	public void update(long bytes, long packets, long errors,
						long drops, long multicast)
	{
		_bytes.update(bytes);
		_packets.update(packets);
		_errors.update(errors);
		_drops.update(drops);
		_multicast.update(multicast);
	}
	
	public void clear()
	{
		_bytes.clear();
		_packets.clear();
		_errors.clear();
		_drops.clear();
		_multicast.clear();
	}
	
	public String toString()
	{
		String output = "bytes: " + _bytes.total();
		output += " packets: " + _packets.total();
		output += " errors: " + _errors.total();
		output += " drops: " + _drops.total();
		if (_multicast.total() > 0)
			output += " multicast: " + _multicast.total();
		
		return output + "\n";
	}
	
}
