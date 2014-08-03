package jsrl.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;

import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EtchedBorder;
import javax.swing.border.TitledBorder;


/**
 * @author ians
 * 
 * 
 */
public class MonitorPanel extends JPanel implements MouseListener
{
	private static final long serialVersionUID = 1L;
	static JCheckBox dateStampCB = new JCheckBox("Output Date Stamp");
	public Monitor monitor;
	JPanel controls;
	boolean doControls;
	JTextField tf;	

	public MonitorPanel(String title, Monitor surf)
	{
		setLayout(new BorderLayout());
		this.monitor = surf;
		setBorder(new TitledBorder(new EtchedBorder(), title));
		add(surf);
		
		controls = new JPanel();
		controls.setPreferredSize(new Dimension(135, 80));
		Font font = new Font("serif", Font.PLAIN, 10);
		JLabel label = new JLabel("Sample Rate");
		label.setFont(font);
		label.setForeground(Color.black);
		controls.add(label);
		tf = new JTextField("1000");
		tf.setPreferredSize(new Dimension(45, 20));
		controls.add(tf);
		controls.add(label = new JLabel("ms"));
		label.setFont(font);
		label.setForeground(Color.black);
		controls.add(dateStampCB);
		dateStampCB.setFont(font);
		
	}
	
	public void start()
	{
		monitor.start();
	}
	
	public void stop()
	{
		monitor.stop();
	}
	
	public void mouseClicked(MouseEvent e)
	{
		removeAll();
		doControls = !doControls;
		if (doControls)
		{
			monitor.stop();
			add(controls);
		} else
		{
			try
			{
				monitor.sleepAmount = Long.parseLong(tf.getText().trim());
			} catch (Exception ex)
			{
			}
			monitor.start();
			add(monitor);
		}
		validate();
		repaint();
	}


	public void mousePressed(MouseEvent e)
	{

	}


	public void mouseReleased(MouseEvent e)
	{

	}


	public void mouseEntered(MouseEvent e)
	{

	}


	public void mouseExited(MouseEvent e)
	{

	}

	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		final MonitorPanel demo = new MonitorPanel("CPU", new Monitor());
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
		JFrame f = new JFrame("MemoryMonitor");
		f.addWindowListener(l);
		f.getContentPane().add("Center", demo);
		f.pack();
		f.setSize(new Dimension(200, 200));
		f.setVisible(true);
		demo.monitor.start();
	}
	
}
