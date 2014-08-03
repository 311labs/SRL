package jsrl.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.net.SocketException;
import java.net.SocketTimeoutException;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import jsrl.net.JavaPing;
import jsrl.net.PingListener;
import jsrl.net.PingResult;

public class PingPanel extends JPanel 
implements ActionListener, MouseListener, WindowListener, PingListener, Runnable
{

	protected JTextArea output = new JTextArea();
	protected JTextField destination = new JTextField();
	protected JButton pingBtn = new JButton();
	protected int seq = 0;
	
	public PingPanel()
	{
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

		JPanel top_panel = new JPanel();
		top_panel.setLayout(new BoxLayout(top_panel, BoxLayout.X_AXIS));
		top_panel.setPreferredSize(new Dimension(380, 20));
		top_panel.setMinimumSize(new Dimension(180, 20));
		top_panel.setMaximumSize(new Dimension(380, 20));		

		JLabel lbl = new JLabel("destination:", SwingConstants.RIGHT);
		top_panel.add(lbl);
		top_panel.add(destination);
		top_panel.add(pingBtn);
		pingBtn.setText("GO");
		pingBtn.setActionCommand("ping");
		pingBtn.addActionListener(this);
		add(top_panel);
		
		output.setEditable(false);
		output.setBackground(Color.black);
		output.setForeground(Color.green);
		add(output);
	}

	public void run() {
		// TODO Auto-generated method stub
		output.setText("PINGING: ");
		String[] dest = destination.getText().split(" ");
        for (int i =0; i < dest.length; ++i)
        {
            output.append(" " + dest[i]);
        }
        output.append("\n");
		float rtt_avg = 0.0f;
		int recv_pings = 0;
		int sent_pings = 0;
		
		float min_rtt=5000.0f;
		float max_rtt=0.0f;
		float avg_rtt=0.0f;

        try {
            JavaPing.getInstance().startListening(this);
            int seq = 0;
            for (int i=0; i < 10; ++i)
            {
                for (int p=0; p < dest.length; ++p) 
                {
                    ++seq;
                    JavaPing.getInstance().ping(dest[p], seq, 56);
                }
                Thread.sleep(1000);
            }
            Thread.sleep(5000);
        } catch (SocketException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
            stop();
            return;
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }        
		
	}

		
	public void run2() {
/*      output.setText("PING " + dest + "\n");
        for (int i=0; i < 10; ++i)
        {
            PingResult result;
            try {
                ++sent_pings;
                result = JavaPing.getInstance().pingpong(dest, i, 56, 5000);
                ping_result(result);
                rtt_avg += result.rtt;
                
                if (result.rtt < min_rtt)
                {
                    min_rtt = result.rtt;
                }
                
                if (result.rtt > max_rtt)
                {
                    max_rtt = result.rtt;
                }               
                
                ++recv_pings;
                
                if (pingBtn.getActionCommand().compareToIgnoreCase("ping") == 0)
                {
                    stop();
                    //JavaPing.getInstance().stopListening();
                    return;
                }
                    
                Thread.sleep(1000);
            } catch (SocketTimeoutException e) {
                // TODO Auto-generated catch block
                output.append("ping request timed out!\n");
            } catch (SocketException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
        
        avg_rtt = (rtt_avg/recv_pings);
        output.append(String.format("--------\n%d packets transmitted, %d packets received, %d%% packet loss\n", sent_pings, recv_pings, 100-(recv_pings*100/sent_pings)));
        output.append(String.format("round-trip min/max/avg %f/%f/%f ms\n", min_rtt, max_rtt, avg_rtt));
        
//      try {
//          JavaPing.getInstance().stopListening();         
//      } catch (SocketException e) {
//          // TODO Auto-generated catch block
//      }   
        stop();*/
	}
	
	public void start()
	{
		pingBtn.setText("stop");
		pingBtn.setActionCommand("stop");
		
		Thread t = new Thread(this);
		t.setDaemon(true);
		t.start();
	}
	
	public void stop()
	{
		pingBtn.setText("ping");
		pingBtn.setActionCommand("ping");	
	}
	

	public void ping_result(PingResult result) {
		if (result.type == JavaPing.ECHO_REPLY)
		{
			String str_result = String.format("%d bytes from %s icmp_seq=%d ttl=%d time=%f ms\n", 
					result.bytes, result.from, result.seq, result.ttl, result.rtt);
			output.append(str_result);
		}
		else if (result.type == JavaPing.DESTINATION_UNREACHABLE)
		{
			String str_result = String.format("%s destination unreachable icmp_seq=%d\n", 
					result.from, result.seq);
			output.append(str_result);			
		}
	}
	
	
	
	public void actionPerformed(ActionEvent event) {
		if (event.getActionCommand().compareTo("ping") == 0)
		{
			start();

		}
		else if (event.getActionCommand().compareTo("stop") == 0)
		{
			stop();
		}
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
		
	}

	public void windowClosing(WindowEvent arg0) {
		// TODO Auto-generated method stub
		
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
	
	public static void showWindow(boolean exit_on_close)
	{
		final PingPanel demo = new PingPanel();
		JFrame f = new JFrame("SRL Ping Tool");

		if (exit_on_close)
		{
			WindowListener l = new WindowAdapter()
			{
				public void windowClosing(WindowEvent e)
				{
					System.exit(0);
				}
	
			};
			f.addWindowListener(l);
		}
		f.getContentPane().add("Center", demo);
		f.pack();
		f.setSize(new Dimension(450, 300));
		f.setVisible(true);		
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		showWindow(true);
	}

}
