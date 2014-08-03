package jsrl.gui;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.Toolkit;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.StringSelection;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.List;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;
import javax.swing.table.JTableHeader;

import jsrl.Version;
import jsrl.sys.ProcessInfo;
import jsrl.sys.SystemInfo;

public class ProcessManager extends JPanel implements ActionListener, Runnable,
		MouseListener, WindowListener {
	private static final long serialVersionUID = 1L;

	protected JTable proc_table = null;

	protected boolean _isRunning = true;

	protected ProcessTableModel proc_model = null;

	protected List<ProcessInfoPanel> procinfo_panels = new Vector<ProcessInfoPanel>();

	Font boldFont = null;

	Font plainFont = null;

	final JPopupMenu proc_menu = new JPopupMenu();

	protected JLabel statusLbl = null;

	MonitorPanel cpu_monitor = new MonitorPanel("CPU", new CpuMonitor());

	MonitorPanel mem_monitor = new MonitorPanel("Memory", new MemoryMonitor());

	MonitorPanel net_monitor = new MonitorPanel("Net", new NetMonitor());

	TableSorter sorter;

	public ProcessManager() {
		// BorderLayout layout = new BorderLayout(5, 10);
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
//		JMenuItem mitem = new JMenuItem("CPU Info");
//		mitem.setMnemonic('I');
//		view_menu.add(mitem);
		
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
		proc_panel.setBorder(new TitledBorder(new EtchedBorder(),
				"Process List"));
		proc_panel.setPreferredSize(new Dimension(150, 300));

		proc_model = new ProcessTableModel();
		sorter = new TableSorter(proc_model);

		proc_table = new JTable(sorter);

		// Get the table header from the table.
		JTableHeader header = proc_table.getTableHeader();

		// Set the table's header in the TableSorter so it knows to
		// listen to mouse clicks and change the appearance of the
		// table header when necessary.
		sorter.setTableHeader(header);

		JScrollPane scrollPane = new JScrollPane(proc_table);
		proc_table.setPreferredScrollableViewportSize(new Dimension(500, 70));

		// proc_panel.add(proc_table.getTableHeader(), BorderLayout.PAGE_START);
		// proc_panel.add(proc_table, BorderLayout.CENTER);
		proc_panel.add(scrollPane);
		proc_table.addMouseListener(this);

		statusLbl = new JLabel("Processes");

		add(proc_panel);
		add(statusLbl);

		JMenuItem menu_item = new JMenuItem("Show Details");
		menu_item.addActionListener(this);
		proc_menu.add(menu_item);

		menu_item = new JMenuItem("Copy");
		menu_item.addActionListener(this);
		proc_menu.add(menu_item);		
		
		proc_menu.addSeparator();

		menu_item = new JMenuItem("Set CPU Mask");
		menu_item.addActionListener(this);
		proc_menu.add(menu_item);

		proc_menu.addSeparator();

		menu_item = new JMenuItem("Cancel Process");
		menu_item.addActionListener(this);
		proc_menu.add(menu_item);

		menu_item = new JMenuItem("Kill Process");
		menu_item.addActionListener(this);
		proc_menu.add(menu_item);

		cpu_monitor.start();
		mem_monitor.start();
		net_monitor.start();

		for (int i = 0; i < 8; ++i) {
			switch (i) {
			case (0):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(50);
				break;
			case (1):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(1);
				break;
			case (2):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(1);
				break;
			case (3):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(1);
				break;
			case (4):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(1);
				break;
			case (5):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(1);
				break;
			case (6):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(1);
				break;
			case (7):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(1);
				break;
			case (8):
				proc_table.getColumnModel().getColumn(i).setPreferredWidth(10);
				break;
			}
		}

		this.setBounds(400, 400, 400, 400);

	}

	public void start() {
		Thread thread = new Thread(this);
		thread.setPriority(Thread.MIN_PRIORITY);
		thread.setName("ProcessManager");
		thread.start();
	}

	
	public JMenuBar buildMenuBar()
	{
		// build menu
		JMenu view_menu = new JMenu("View");
		view_menu.setMnemonic('V');
		
		JMenuItem mitem = new JMenuItem("CPU Info");
		mitem.setActionCommand("cpu info");
		mitem.addActionListener(this);
		mitem.setMnemonic('I');
		view_menu.add(mitem);
		
		mitem = new JMenuItem("Ping Tool");
		mitem.setActionCommand("ping tool");
		mitem.addActionListener(this);		
		mitem.setMnemonic('P');
		view_menu.add(mitem);
		
		JMenuBar bar = new JMenuBar();
		bar.add(view_menu);
		return bar;		
	}
	
	public void run() {
		while (_isRunning) {
			SystemInfo.Sleep(2000);
			proc_model.update();

			try {
				for (ProcessInfoPanel child : procinfo_panels) {
					child.update();
				}
			} catch (Exception e) {

			}
			statusLbl.setText(proc_table.getRowCount() + " Processes");
			// sorter.resort();
			proc_table.repaint();
			System.gc();
		}
	}

	public void actionPerformed(ActionEvent e) {
		String action = e.getActionCommand();
	
		if (action.equalsIgnoreCase("ping tool")) {
			PingPanel.showWindow(false);
		}
		else if (action.equalsIgnoreCase("cpu info")) {
			((CpuMonitor)cpu_monitor.monitor).showCpuInfo();
		}
		else if (action.equalsIgnoreCase("show details")) {
			int index = proc_table.getSelectedRow();
			int pid = (Integer)proc_table.getValueAt(index, 1);
			//System.out.println();
			if (index >= 0) {
				ProcessInfo proc_info = proc_model.procMan.getProcess(pid);
				if (proc_info != null)
					showProcInfo(proc_info);
				// repaint();
			}
		} else if (action.equalsIgnoreCase("set cpu mask")) {

		} else if (action.equalsIgnoreCase("cancel process")) {
			int index = proc_table.getSelectedRow();
			ProcessInfo proc_info = proc_model.getProcessInfo(index);
			proc_info.terminate();

		} else if (action.equalsIgnoreCase("kill process")) {
			int index = proc_table.getSelectedRow();
			ProcessInfo proc_info = proc_model.getProcessInfo(index);
			proc_info.terminate();
		} else if (action.equalsIgnoreCase("copy")) {
			StringBuffer sbf = new StringBuffer();
			// Check to ensure we have selected only a contiguous block of
			// cells
			int numcols = proc_table.getSelectedColumnCount();
			int numrows = proc_table.getSelectedRowCount();
			int[] rowsselected = proc_table.getSelectedRows();
			int[] colsselected = proc_table.getSelectedColumns();
			if (!((numrows - 1 == rowsselected[rowsselected.length - 1]
					- rowsselected[0] && numrows == rowsselected.length) && (numcols - 1 == colsselected[colsselected.length - 1]
					- colsselected[0] && numcols == colsselected.length))) {
				JOptionPane.showMessageDialog(null, "Invalid Copy Selection",
						"Invalid Copy Selection", JOptionPane.ERROR_MESSAGE);
				return;
			}
			for (int i = 0; i < numrows; i++) {
				for (int j = 0; j < numcols; j++) {
					sbf.append(proc_table.getValueAt(rowsselected[i],
							colsselected[j]));
					if (j < numcols - 1)
						sbf.append("\t");
				}
				sbf.append("\n");
			}
			StringSelection stsel = new StringSelection(sbf.toString());
			Clipboard sys_clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
			sys_clipboard.setContents(stsel, stsel);
		}
	}

	public void showProcInfo(ProcessInfo proc_info) {
		ProcessInfoPanel new_child = null;
		for (ProcessInfoPanel child : procinfo_panels) {
			if (child.proc_info.id() == proc_info.id()) {
				new_child = child;

			}
		}

		if (new_child == null) {
			new_child = new ProcessInfoPanel(proc_info);
			JFrame f = new JFrame(String.format("%s [%d]", proc_info.name(),
					proc_info.id()));
			f.addWindowListener(this);
			f.addWindowListener(new_child);
			f.getContentPane().add("Center", new_child);
			f.pack();
			f.setSize(new Dimension(600, 400));
			f.setVisible(true);
			new_child.parent_window = f;
			procinfo_panels.add(new_child);

		} else {
			new_child.parent_window.toFront();
		}
	}

	public void mouseClicked(MouseEvent e) {
		// TODO Auto-generated method stub
		// int viewColumn =
		// proc_table.getTableHeader().getColumnModel().getColumnIndexAtX(e.getX());
		// proc_model.sortByColumn(viewColumn);
		if ((e.getClickCount() > 1) && (e.getButton() == 1)) {
			int index = proc_table.getSelectedRow();
			int pid = (Integer)proc_table.getValueAt(index, 1);
			//System.out.println();
			if (index >= 0) {
				ProcessInfo proc_info = proc_model.procMan.getProcess(pid);
				if (proc_info != null)
					showProcInfo(proc_info);
			}
		}
	}

	public void mousePressed(MouseEvent evt) {
		if (evt.isPopupTrigger()) {
			proc_menu.show(evt.getComponent(), evt.getX(), evt.getY());
		}
	}

	public void mouseReleased(MouseEvent evt) {

	}

	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		final ProcessManager manager = new ProcessManager();
		WindowListener l = new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				// manager.stop_thread = true;
				manager._isRunning = false;
				System.exit(0);
			}

			public void windowDeiconified(WindowEvent e) {
				// demo.surf.start();
			}

			public void windowIconified(WindowEvent e) {
				// demo.surf.stop();
			}
		};
		JFrame f = new JFrame("SRL - System Information - SRL Version: "
				+ Version.MAJOR + "." + Version.MINOR + "." + Version.REVISION);
		f.addWindowListener(l);
		f.getContentPane().add("Center", manager);
		f.setJMenuBar(manager.buildMenuBar());
		f.pack();
		f.setSize(new Dimension(800, 600));
		f.setVisible(true);
		manager.start();
	}

	public void windowActivated(WindowEvent arg0) {
		// TODO Auto-generated method stub

	}

	public void windowClosed(WindowEvent arg0) {
		// TODO Auto-generated method stub
		for (ProcessInfoPanel child : procinfo_panels) {
			if (child.is_open == false) {
				//System.out.println("removing dead window");
				procinfo_panels.remove(child);
				break;
			}
		}
	}

	public void windowClosing(WindowEvent event) {
		// TODO Auto-generated method stub
		//System.out.println("window closing?");
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
