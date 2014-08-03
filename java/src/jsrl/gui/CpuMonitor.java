package jsrl.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.List;
import java.util.Vector;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

import jsrl.gui.Monitor.Data;
import jsrl.sys.SystemInfo;

public class CpuMonitor extends Monitor implements MouseListener
{
	private static final long serialVersionUID = 1L;
	
	protected Data cpu_total = new Data();
	protected Data cpu_user = new Data();
	protected Data cpu_kernel = new Data();
	
	protected List<Data> cpus = new Vector<Data>();
	
	public CpuMonitor()
	{
		cpu_user.show_bar = false;
		cpu_user.color = Color.WHITE;
		
		cpu_kernel.show_bar = false;
		cpu_kernel.color = Color.RED;

		SystemInfo sysinfo = SystemInfo.getInstance();
		header = sysinfo.cpu().modelName() + " x " + sysinfo.cpuCount();
		footer = "";
		add(cpu_total);
		add(cpu_user);
		add(cpu_kernel);
	
//		if (sysinfo.cpuCount() > 1)
//		{
//			for (int i=0;i<sysinfo.cpuCount();++i)
//			{
//				Data cpu = new Data();
//				cpu.show_plot = false;
//				cpus.add(cpu);
//				add(cpu);
//			}
//		}
		
		
		this.addMouseListener(this);
	}
	
	protected void update()
	{
		SystemInfo sysinfo = SystemInfo.getInstance();
		sysinfo.update();
		
		footer = String.format(" user: %d%%  kernel: %d%%", 
				(int)sysinfo.cpu().userLoad(),(int)sysinfo.cpu().kernelLoad());
		
		cpu_total.add(sysinfo.cpu().load());
		cpu_user.add(sysinfo.cpu().userLoad());
		cpu_kernel.add(sysinfo.cpu().kernelLoad());
		
		if (lbl_freq != null)
		{
			lbl_freq.setText("" + sysinfo.cpu().speed());
		}
		
		int pos = 0;
		for (Data cpu : cpus)
		{
			cpu.add(sysinfo.getCPU(pos).load());
			++pos;
		}
		
	}
	
	protected JFrame cpu_info_window = null;
	protected JLabel lbl_freq = null;
	public void showCpuInfo()
	{
		if (cpu_info_window == null)
		{
			JPanel panel = new JPanel();
	        panel.setLayout(new GridBagLayout());
	        GridBagConstraints c = new GridBagConstraints();		
			
	        c.fill = GridBagConstraints.HORIZONTAL;
	       
	        SystemInfo sysinfo = SystemInfo.getInstance();
	        int y = 0;
	        
	        JLabel lbl = new JLabel("Vendor ID:", SwingConstants.RIGHT);
	        c.gridx = 0;
	        c.gridy = y;
	        c.weightx = 1.0;
	        panel.add(lbl, c);
	        
	        JLabel value = new JLabel(sysinfo.cpu().vendor());
	        value.setForeground(cpu_total.color);
	        c.gridx = 1;
	        c.gridy = y;
	        c.gridwidth = 3;
	        panel.add(value, c);        

	        lbl = new JLabel("Freq:", SwingConstants.RIGHT);
	        c.gridx = 2;
	        c.gridy = y;
	        c.gridwidth = 1;
	        panel.add(lbl, c);
	        lbl_freq = new JLabel("" + sysinfo.cpu().speed());
	        value.setForeground(cpu_total.color);
	        c.gridwidth = 1;
	        c.gridx = 3;
	        c.gridy = y;
	        panel.add(lbl_freq, c);  	        
	        
	        ++y;
	        lbl = new JLabel("Model Name:", SwingConstants.RIGHT);
	        c.gridx = 0;
	        c.gridy = y;
	        c.gridwidth = 1;
	        panel.add(lbl, c);
	        value = new JLabel(sysinfo.cpu().modelName());
	        value.setForeground(cpu_total.color);
	        c.gridwidth = 5;
	        c.gridx = 1;
	        c.gridy = y;
	        panel.add(value, c);        
	
	        ++y;
	        lbl = new JLabel("Family:", SwingConstants.RIGHT);
	        c.gridx = 0;
	        c.gridy = y;
	        c.gridwidth = 1;
	        panel.add(lbl, c);
	        value = new JLabel("" + sysinfo.cpu().family());
	        value.setForeground(cpu_total.color);
	        c.gridwidth = 1;
	        c.gridx = 1;
	        c.gridy = y;
	        panel.add(value, c);  
	        
	        lbl = new JLabel("Model:", SwingConstants.RIGHT);
	        c.gridx = 2;
	        c.gridy = y;
	        c.gridwidth = 1;
	        panel.add(lbl, c);
	        value = new JLabel("" + sysinfo.cpu().model());
	        value.setForeground(cpu_total.color);
	        c.gridwidth = 1;
	        c.gridx = 3;
	        c.gridy = y;
	        panel.add(value, c);  
	        
	        lbl = new JLabel("Type:", SwingConstants.RIGHT);
	        c.gridx = 4;
	        c.gridy = y;
	        c.gridwidth = 1;
	        panel.add(lbl, c);
	        value = new JLabel("" + sysinfo.cpu().type());
	        value.setForeground(cpu_total.color);
	        c.gridwidth = 5;
	        c.gridx = 5;
	        c.gridy = y;
	        panel.add(value, c);  
	        
	        ++y;
	        lbl = new JLabel("Hyper-Threading Support:", SwingConstants.RIGHT);
	        c.gridx = 0;
	        c.gridy = y;
	        c.gridwidth = 2;
	        panel.add(lbl, c);
	        value = new JLabel("" + sysinfo.isHtSupported());
	        value.setForeground(cpu_total.color); 
	        c.gridwidth = 1;
	        c.gridx = 2;
	        c.gridy = y;
	        panel.add(value, c);     
	
	        lbl = new JLabel("Hyper-Threading Enabled:", SwingConstants.RIGHT);
	        c.gridx = 2;
	        c.gridy = y;
	        c.gridwidth = 2;
	        panel.add(lbl, c);
	        value = new JLabel("" + sysinfo.isHtEnabled());
	        value.setForeground(cpu_total.color);
	        c.gridx = 4;
	        c.gridy = y;
	        c.gridwidth = 1;
	        panel.add(value, c);           
	        
	        ++y;
	        lbl = new JLabel("Logical Processors:", SwingConstants.RIGHT);
	        c.gridx = 0;
	        c.gridy = y;
	        panel.add(lbl, c);
	        value = new JLabel("" + sysinfo.numberOfLogicalProcessors());
	        value.setForeground(cpu_total.color);
	        c.gridx = 1;
	        c.gridy = y;
	        panel.add(value, c); 
	        
	        lbl = new JLabel("Physical Processors:", SwingConstants.RIGHT);
	        c.gridx = 2;
	        c.gridy = y;
	        panel.add(lbl, c);
	        value = new JLabel("" + sysinfo.numberOfPhysicalProcessors());
	        value.setForeground(cpu_total.color);
	        c.gridx = 3;
	        c.gridy = y;
	        panel.add(value, c); 
	        
	        lbl = new JLabel("Cores Per Processor:", SwingConstants.RIGHT);
	        c.gridx = 4;
	        c.gridy = y;
	        panel.add(lbl, c);
	        value = new JLabel("" + sysinfo.numberOfCoresPerProcessor());
	        value.setForeground(cpu_total.color);
	        c.gridx = 5;
	        c.gridy = y;
	        panel.add(value, c); 
	        
	//        lbl = new JLabel("Signature:");
	//        c.gridx = 0;
	//        c.gridy = y;
	//        panel.add(lbl, c);
	//        value = new JLabel(sysinfo.cpu().model());
	//        c.gridx = 1;
	//        c.gridy = y;
	//        panel.add(value, c);         
	
			
	        cpu_info_window = new JFrame("CPU Info");
			//f.addWindowListener(panel);
	        cpu_info_window.getContentPane().add("Center", panel);
	        cpu_info_window.pack();
	        cpu_info_window.setSize(new Dimension(620, 120));
	        cpu_info_window.setVisible(true);		
	
		}
		else
		{
			cpu_info_window.setVisible(true);
			cpu_info_window.toFront();
		}
	}
	public void mouseClicked(MouseEvent evt) {
		// TODO Auto-generated method stub
		if (evt.getButton() > 1)
			showCpuInfo();
		
	}

	public void mouseEntered(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	public void mouseExited(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	public void mousePressed(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	public void mouseReleased(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
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
