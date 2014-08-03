package jsrl.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.JFrame;

import jsrl.sys.SystemInfo;

public class CpuMonitor extends Monitor
{
	private static final long serialVersionUID = 1L;
	
	protected Data cpu_total = new Data();
	//protected Data cpu_kernel = new Data();
	
	public CpuMonitor()
	{
		//cpu_kernel.color = Color.RED;

		SystemInfo sysinfo = SystemInfo.getInstance();
		header = sysinfo.cpu().model() + " x " + sysinfo.cpuCount();
		footer = "";
		add(cpu_total);
		//add(cpu_kernel);
	}
	
	protected void update()
	{
		SystemInfo sysinfo = SystemInfo.getInstance();
		sysinfo.update();
		
		footer = String.format(" user: %d%%  kernel: %d%%", 
				(int)sysinfo.cpu().userLoad(),(int)sysinfo.cpu().kernelLoad());
		
		cpu_total.add(sysinfo.cpu().load());
		//cpu_kernel.add(sysinfo.cpu().kernelLoad());
	}
	
	
	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		final MonitorPanel demo = new MonitorPanel("CPU", new CpuMonitor());
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
		JFrame f = new JFrame("CPU Monitor");
		f.addWindowListener(l);
		f.getContentPane().add("Center", demo);
		f.pack();
		f.setSize(new Dimension(200, 200));
		f.setVisible(true);
		demo.monitor.start();
	}

}
