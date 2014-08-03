package jsrl.gui;


import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.List;

import javax.swing.JFrame;

import jsrl.net.NetworkInterface;

public class NetMonitor extends Monitor
{

	private static final long serialVersionUID = 1L;
	NetworkInterface iface = null;
	private float rx_bytes = 0;
	private float tx_bytes = 0;
	
	protected Data net_tx = new Data();
	protected Data net_rx = new Data();
	
	public NetMonitor()
	{
		List<NetworkInterface> all = NetworkInterface.getAll();

		for (NetworkInterface nface : all)
		{
			if (!nface.name().equalsIgnoreCase("lo") && (nface.ip().length() > 4))
			{
				this.iface = nface;
				break;
			}
				
		}
		
		init();
	}
	

	
	public NetMonitor(NetworkInterface iface)
	{
		this.iface = iface;
		init();
	}
	
	public void init()
	{
		iface.clear();
		rx_bytes = 0;
		
		header = iface.name() + "   ip: " + iface.ip();
		
		net_tx.color = Color.BLUE;
		
		add(net_rx);
		add(net_tx);
		
		net_tx.max_value = 1024.0f;
		net_rx.max_value = 1024.0f;
	}
	
	protected void update()
	{
		NetworkInterface.updateList();
		
		float cur_rx_bytes = iface.rx().bytes()/1024.0f;
		float rxvalue = cur_rx_bytes - rx_bytes;
		rx_bytes = cur_rx_bytes;
		net_rx.add(rxvalue);
		
		float cur_tx_bytes = iface.tx().bytes()/1024.0f;
		float txvalue = cur_tx_bytes - tx_bytes;
		tx_bytes = cur_tx_bytes;
		net_tx.add(txvalue);
		
		// TODO put a real timer in here
		float tx_bytes_sec = txvalue;
		float rx_bytes_sec = rxvalue;

		footer = String.format(" rx: %.0f kb/s  tx: %.0f kb/s", 
				rx_bytes_sec, tx_bytes_sec);

	}

	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		
		final MonitorPanel demo = new MonitorPanel("Net", new NetMonitor());
		WindowListener l = new WindowAdapter()
		{
			public void windowClosing(WindowEvent e)
			{
				System.exit(0);
			}

			public void windowDeiconified(WindowEvent e)
			{
				demo.monitor.start();
			}

			public void windowIconified(WindowEvent e)
			{
				demo.monitor.stop();
			}
		};
		JFrame f = new JFrame("NET Monitor");
		f.addWindowListener(l);
		f.getContentPane().add("Center", demo);
		f.pack();
		f.setSize(new Dimension(200, 200));
		f.setVisible(true);
		demo.monitor.start();
	}

}
