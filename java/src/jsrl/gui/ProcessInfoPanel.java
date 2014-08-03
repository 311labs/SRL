package jsrl.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.SwingConstants;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;

import jsrl.sys.ProcessInfo;
import jsrl.sys.SystemInfo;

public class ProcessInfoPanel  extends JPanel 
implements ActionListener, MouseListener, WindowListener
{
	public class ProcessCpuMonitor extends Monitor
	{
		private static final long serialVersionUID = 1L;
		
		public ProcessInfo proc_info = null;
		protected Data cpu_total = new Data();
		protected Data cpu_user = new Data();
		protected Data cpu_kernel = new Data();
		
		public ProcessCpuMonitor(ProcessInfo proc_info)
		{
			this.proc_info = proc_info;
			
			//cpu_user.show_bar = false;
			cpu_user.color = Color.WHITE;
			
			//cpu_kernel.show_bar = false;
			cpu_kernel.color = Color.RED;			
			
			SystemInfo sysinfo = SystemInfo.getInstance();
			header = sysinfo.cpu().modelName() + " x " + sysinfo.cpuCount();
			footer = "";
			add(cpu_total);
			add(cpu_user);
			add(cpu_kernel);
			
		}
		
		protected void update()
		{
			//footer = String.format(" user: %d%%  kernel: %d%%", 
			//		(int)sysinfo.cpu().userLoad(),(int)sysinfo.cpu().kernelLoad());
			
			cpu_total.add(proc_info.cpu().load());
			cpu_user.add(proc_info.cpu().userLoad());
			cpu_kernel.add(proc_info.cpu().kernelLoad());
		}
	}
	
	public class ProcessMemoryMonitor extends Monitor
	{
		private static final long serialVersionUID = 1L;
		
		protected Data sys_mem = new Data();
		protected ProcessInfo proc_info = null;
		
		public ProcessMemoryMonitor(ProcessInfo proc_info)
		{
			this.proc_info = proc_info;
			SystemInfo sysinfo = SystemInfo.getInstance();
			header = String.format(" Total   Ram: %.2f mb   Swap: %.2f mb", 
					sysinfo.memory().total() / 1024.0f,
					sysinfo.swap().total() / 1024.0f);
			sys_mem.max_value = sysinfo.memory().total() / 1024.0f;

			add(sys_mem);
		}
		
		protected void update()
		{
			
			footer = String.format("Process Memory: %.0f mb  faults: %d", 
					proc_info.memory().used()/1024.0f/1024.0f, proc_info.memory().faults());
			sys_mem.add(proc_info.memory().used()/1024.0f/1024.0f);
		}
	}
	
	
	MonitorPanel cpu_monitor = null;
	MonitorPanel mem_monitor = null;
	MonitorPanel net_monitor = new MonitorPanel("Net", new NetMonitor());
	
	protected ProcessInfo proc_info = null;
	protected boolean is_open = true;
	protected JLabel lbl_name = new JLabel();
	protected JLabel lbl_pid = new JLabel();
	protected JLabel lbl_parent = new JLabel();
	protected JLabel lbl_threads = new JLabel();
	protected JLabel lbl_handles = new JLabel();
	protected JTextArea lbl_command = new JTextArea(200,4);
	protected JLabel lbl_filename = new JLabel();
	protected JLabel lbl_owner = new JLabel();
	protected JLabel lbl_priority = new JLabel();
	protected int handles, threads, priority;
	
	public JFrame parent_window = null;
	
	public ProcessInfoPanel(ProcessInfo proc_info)
	{
		this.proc_info = proc_info;
		
		String[] labels = {"name:", "pid:", "filename:", "owner:", "parent:", "threads:", "handles:", "priority:"};
		JLabel[] fields = {lbl_name, lbl_pid, lbl_filename, lbl_owner, lbl_parent, lbl_threads, lbl_handles, lbl_priority};
		//BorderLayout layout = new BorderLayout(5, 10);
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		// top panel
		JPanel top_panel = new JPanel();
		top_panel.setLayout(new BoxLayout(top_panel, BoxLayout.X_AXIS));
		top_panel.setPreferredSize(new Dimension(400, 200));
		top_panel.setMinimumSize(new Dimension(250, 200));
		top_panel.setMaximumSize(new Dimension(1280, 400));
		
		cpu_monitor = new MonitorPanel("CPU", new ProcessCpuMonitor(proc_info));
		mem_monitor = new MonitorPanel("Memory", new ProcessMemoryMonitor(proc_info));
		
		cpu_monitor.setPreferredSize(new Dimension(250, 200));
		cpu_monitor.setSize(150, 100);
		mem_monitor.setPreferredSize(new Dimension(250, 200));
		mem_monitor.setSize(150, 100);
		
		top_panel.add(cpu_monitor);
		top_panel.add(mem_monitor);
		
		add(top_panel);
		
		// top panel		
		JPanel proc_panel = new JPanel();
		BoxLayout infoLayout = new BoxLayout(proc_panel, BoxLayout.Y_AXIS);
		proc_panel.setLayout(infoLayout);
		proc_panel.setBorder(new TitledBorder(new EtchedBorder(), "Process Info"));
		proc_panel.setPreferredSize(new Dimension(50, 200));
		//proc_panel.setSize(50, 100);		
		proc_panel.setMinimumSize(new Dimension(50, 100));
		proc_panel.setMaximumSize(new Dimension(1280, 400));
		//proc_panel.setAlignmentX(LEFT_ALIGNMENT);
		
		lbl_name.setText(" " + proc_info.name());
		lbl_pid.setText(" " + proc_info.id());
		lbl_filename.setText(" " + proc_info.filename());
		lbl_command.setText(" " + proc_info.command());
		lbl_command.setWrapStyleWord(true);
		lbl_command.setLineWrap(true);

		lbl_command.setPreferredSize(new Dimension(400, 4));
		lbl_command.setMinimumSize(new Dimension(250, 2));
		lbl_command.setMaximumSize(new Dimension(1280, 8));		
		
		if (proc_info.parent() != null)
			lbl_parent.setText(String.format(" %s [%d]", proc_info.parent().name(), proc_info.parentId()));
		lbl_owner.setText(" " + proc_info.owner());
		lbl_threads.setText(" " + proc_info.threads());
		lbl_handles.setText(" " + proc_info.handles());
		lbl_priority.setText(" " + proc_info.priority());
		JPanel info_panel = null;
		for (int i=0; i<labels.length;++i)
		{
			if ((i == 0) || (i == 2) || (i == 3) || (i == 5))
			{
				info_panel = new JPanel();
				//info_panel.setBackground(Color.blue);
				info_panel.setLayout(new FlowLayout(FlowLayout.LEFT, 5, 0));		
			}
			JLabel lbl = new JLabel(labels[i], SwingConstants.RIGHT);
			//lbl.setPreferredSize(new Dimension(20, 50));
			//lbl.setSize(new Dimension(20, 50));
			info_panel.add(lbl);
			
			fields[i].setForeground(Color.BLUE);
			info_panel.add(fields[i]);;
			if ((i == 0) || (i == 2) || (i == 3) || (i == 5))
			{
				proc_panel.add(info_panel);
			}
			
			if ((i == 0) || (i == 3) || (i == 5) || (i == 6))
			{
				info_panel.add(new JLabel("      "));
			}
		}
		
		proc_panel.add(lbl_command);
		
		this.setAlignmentX(LEFT_ALIGNMENT);
		add(proc_panel);

		cpu_monitor.start();
		mem_monitor.start();
		
		this.setBounds(400,400,400,400);
	}
	
	
	public void update()
	{
		if (handles != proc_info.handles())
		{
			handles = proc_info.handles();
			lbl_handles.setText(" " + handles);
		}
		
		if (threads != proc_info.threads())
		{
			threads = proc_info.threads();
			lbl_threads.setText(" " + threads);
		}
		
		if (priority != proc_info.priority())
		{
			priority = proc_info.priority();
			lbl_priority.setText(" " + priority);
		}		
	}
	
	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	public void run() {
		// TODO Auto-generated method stub
		
	}

	public void mouseClicked(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
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


	public void windowActivated(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}


	public void windowClosed(WindowEvent arg0) {
		// TODO Auto-generated method stub
		//System.out.println("panel got closed");
		cpu_monitor.stop();
		is_open = false;		
	}


	public void windowClosing(WindowEvent arg0) {
		// TODO Auto-generated method stub
		//System.out.println("panel got closing");
		cpu_monitor.stop();
		is_open = false;
		arg0.getWindow().dispose();
	}


	public void windowDeactivated(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}


	public void windowDeiconified(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}


	public void windowIconified(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}


	public void windowOpened(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
	}

}
