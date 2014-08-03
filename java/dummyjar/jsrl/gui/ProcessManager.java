package jsrl.gui;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

import jsrl.sys.SystemInfo;


public class ProcessManager extends JPanel 
implements ActionListener, Runnable, MouseListener
{
	private static final long serialVersionUID = 1L;
	protected JTable proc_table = null;
	protected boolean _isRunning = true;
	protected ProcessTableModel proc_model = null;

	protected JLabel statusLbl = null;
	MonitorPanel cpu_monitor = new MonitorPanel("CPU", new CpuMonitor());
	MonitorPanel mem_monitor = new MonitorPanel("Memory", new MemoryMonitor());
	MonitorPanel net_monitor = new MonitorPanel("Net", new NetMonitor());
		
	
	public ProcessManager()
	{
		//BorderLayout layout = new BorderLayout(5, 10);
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		// top panel
		JPanel top_panel = new JPanel();
		top_panel.setLayout(new BoxLayout(top_panel, BoxLayout.X_AXIS));
		top_panel.setPreferredSize(new Dimension(400, 200));
		top_panel.setMinimumSize(new Dimension(150, 100));
		top_panel.setMaximumSize(new Dimension(1280, 100));
		
		cpu_monitor.setPreferredSize(new Dimension(150, 100));
		cpu_monitor.setSize(150, 100);
		mem_monitor.setPreferredSize(new Dimension(150, 100));
		mem_monitor.setSize(150, 100);
		net_monitor.setPreferredSize(new Dimension(150, 100));
		net_monitor.setSize(150, 100);
		
		top_panel.add(cpu_monitor);
		top_panel.add(mem_monitor);
		top_panel.add(net_monitor);
		
		add(top_panel);
		
		// top panel
		JPanel proc_panel = new JPanel();
		proc_panel.setLayout(new BoxLayout(proc_panel, BoxLayout.Y_AXIS));
		proc_panel.setBorder(new TitledBorder(new EtchedBorder(), "Process List"));
		proc_panel.setPreferredSize(new Dimension(150, 300));
		
		proc_model = new ProcessTableModel();
		proc_table = new JTable(new ProcessTableModel());
	
		JScrollPane scrollPane = new JScrollPane(proc_table);
		proc_table.setPreferredScrollableViewportSize(new Dimension(500, 70));		
		
		//proc_panel.add(proc_table.getTableHeader(), BorderLayout.PAGE_START);
		//proc_panel.add(proc_table, BorderLayout.CENTER);
		proc_panel.add(scrollPane);
		
		statusLbl = new JLabel("Processes");
		
		
		add(proc_panel);
		add(statusLbl);
		
		
		proc_table.getTableHeader().addMouseListener(this);
		
		//TableColumn column = null;
		TableColumnModel columns = proc_table.getColumnModel();
		for (int i = 0; i < 8; ++i)
		{
			TableColumn column = columns.getColumn(i);
			switch (i)
			{
			case (0):
				column.setPreferredWidth(50);
				break;
			case (1):
			{
				column.setPreferredWidth(2);
				DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
				renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
				column.setCellRenderer(renderer);			
				break;
			}
			case (2):
			{
				column.setPreferredWidth(2);
				DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
				renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
				column.setCellRenderer(renderer);			
				break;
			}
			case (3):
			{
				column.setPreferredWidth(2);
				DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
				renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
				column.setCellRenderer(renderer);			
				break;
			}
			case (4):
			{
				column.setPreferredWidth(5);
				DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
				renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
				column.setCellRenderer(renderer);				
				break;
			}
			case (5):
			{
				column.setPreferredWidth(5);
				DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
				renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
				column.setCellRenderer(renderer);				
				break;
			}
			case (6):
			{
				column.setPreferredWidth(5);
				DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
				renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
				column.setCellRenderer(renderer);				
				break;
			}
			case (7):
				column.setPreferredWidth(50);
				break;
			}
		}	
		cpu_monitor.start();
		mem_monitor.start();
		net_monitor.start();
		
		this.setBounds(400,400,400,400);
	}
	
	public void start()
	{
		Thread thread = new Thread(this);
		thread.setPriority(Thread.MIN_PRIORITY);
		thread.setName("ProcessManager");
		thread.start();
	}
	
	public void actionPerformed(ActionEvent e)
	{

	}


	public void run()
	{
		while (_isRunning)
		{
			SystemInfo.Sleep(1000);
			proc_model.update();
			proc_table.setModel(proc_model);
			//TableColumn column = null;
			TableColumnModel columns = proc_table.getColumnModel();
			for (int i = 0; i < 8; ++i)
			{
				TableColumn column = columns.getColumn(i);
				switch (i)
				{
				case (0):
					column.setPreferredWidth(50);
					break;
				case (1):
				{
					column.setPreferredWidth(2);
					DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
					renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
					column.setCellRenderer(renderer);			
					break;
				}
				case (2):
				{
					column.setPreferredWidth(2);
					DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
					renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
					column.setCellRenderer(renderer);			
					break;
				}
				case (3):
				{
					column.setPreferredWidth(2);
					DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
					renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
					column.setCellRenderer(renderer);			
					break;
				}
				case (4):
				{
					column.setPreferredWidth(5);
					DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
					renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
					column.setCellRenderer(renderer);				
					break;
				}
				case (5):
				{
					column.setPreferredWidth(5);
					DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
					renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
					column.setCellRenderer(renderer);				
					break;
				}
				case (6):
				{
					column.setPreferredWidth(5);
					DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();
					renderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
					column.setCellRenderer(renderer);				
					break;
				}
				case (7):
					column.setPreferredWidth(50);
					break;
				}
			}		
			statusLbl.setText(proc_table.getRowCount() + " Processes");
			proc_table.repaint();
			//cpu_monitor.monitor.run();
			//cpu_monitor.monitor.run();
			//cpu_monitor.monitor.run();
			
			System.gc();
		}
	}	


	public void mouseClicked(MouseEvent e)
	{
		// TODO Auto-generated method stub
		int viewColumn = proc_table.getTableHeader().getColumnModel().getColumnIndexAtX(e.getX());
		proc_model.sortByColumn(viewColumn);
		//System.out.println("column sort: " + viewColumn);
		repaint();
	}

	public void mousePressed(MouseEvent e)
	{
		// TODO Auto-generated method stub
		
	}

	public void mouseReleased(MouseEvent e)
	{
		// TODO Auto-generated method stub
		
	}

	public void mouseEntered(MouseEvent e)
	{
		// TODO Auto-generated method stub
		
	}

	public void mouseExited(MouseEvent e)
	{
		// TODO Auto-generated method stub
		
	}
	
	
	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		final ProcessManager manager = new ProcessManager();
		WindowListener l = new WindowAdapter()
		{
			public void windowClosing(WindowEvent e)
			{
				//manager.stop_thread = true;	
				manager._isRunning = false;
				System.exit(0);
			}

			public void windowDeiconified(WindowEvent e)
			{
				//				demo.surf.start();
			}

			public void windowIconified(WindowEvent e)
			{
				//				demo.surf.stop();
			}
		};
		JFrame f = new JFrame("SRL - System Information - Version 0.1");
		f.addWindowListener(l);
		f.getContentPane().add("Center", manager);
		f.pack();
		f.setSize(new Dimension(800, 600));
		f.setVisible(true);
		manager.start();
	}	
	
}
