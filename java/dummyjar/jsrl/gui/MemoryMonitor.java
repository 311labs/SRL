package jsrl.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.JFrame;

import jsrl.sys.SystemInfo;

public class MemoryMonitor extends Monitor
{
	private static final long serialVersionUID = 1L;
	
	protected Data sys_mem = new Data();
	protected Data swp_mem = new Data();
	
	
	public MemoryMonitor()
	{
		SystemInfo sysinfo = SystemInfo.getInstance();
		header = String.format(" Total   Ram: %.2f mb   Swap: %.2f mb", 
				sysinfo.memory().total() / 1024.0f,
				sysinfo.swap().total() / 1024.0f);
		sys_mem.max_value = sysinfo.memory().total();
		swp_mem.max_value = sysinfo.swap().total();
		
		swp_mem.color = Color.RED;
		add(sys_mem);
		add(swp_mem);
	}
	
	protected void update()
	{
		SystemInfo sysinfo = SystemInfo.getInstance();
		sysinfo.update();

		
		footer = String.format("Used   Ram: %.0f mb   Swap: %.0f mb", 
				sysinfo.memory().used()/1024.0f, sysinfo.swap().used()/1024.0f);

		sys_mem.add(sysinfo.memory().used());
		swp_mem.add(sysinfo.swap().used());
	}

	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		final MonitorPanel demo = new MonitorPanel("Memory", new MemoryMonitor());
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
		JFrame f = new JFrame("Memory Monitor");
		f.addWindowListener(l);
		f.getContentPane().add("Center", demo);
		f.pack();
		f.setSize(new Dimension(200, 200));
		f.setVisible(true);
		demo.monitor.start();
	}

}
