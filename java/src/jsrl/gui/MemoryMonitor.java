package jsrl.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.GradientPaint;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.TexturePaint;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.image.BufferedImage;

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

	 public static void main(String[] args) {
	      final int WIDTH = 2000;
	      final int HEIGHT = 2000;     
	      final int TIMES = 100;
	      
	      System.out.println("Testing image at " + WIDTH + "x" + HEIGHT + " (" + TIMES + " Times)");
	      
	      GradientPaint grad = new GradientPaint(0, 0, Color.BLACK, 0, HEIGHT, Color.WHITE); 
	      
	      BufferedImage dest = new BufferedImage(WIDTH, HEIGHT, BufferedImage.TYPE_INT_ARGB);
	      Graphics2D destg = dest.createGraphics();
	      
	      long ns = 0, t = 0;
	      
	      // full paint test
	      destg.setPaint(grad);
	      ns = System.nanoTime();
	      for (int i = 0; i < TIMES; i++) {
	         destg.fillRect(0, 0, WIDTH, HEIGHT);
	      }
	      t = System.nanoTime() - ns;
	      System.out.printf("Full Gradient Paint in %.3fns%n", t / (double) TIMES);
	            
	      // setup source image      
	      BufferedImage source = new BufferedImage(1, HEIGHT, BufferedImage.TYPE_INT_ARGB);
	      Graphics2D sourceg = source.createGraphics();
	      sourceg.setPaint(grad);
	      sourceg.fillRect(0, 0, 1, HEIGHT);
	      sourceg.dispose();

	      // stretch
	      ns = System.nanoTime();
	      for (int i = 0; i < TIMES; i++) {
	         destg.drawImage(source, 0, 0, WIDTH, HEIGHT, null);
	      }
	      t = System.nanoTime() - ns;
	      System.out.printf("Stretch Image Paint in %.3fns%n", t / (double) TIMES);

	      // tile
	      destg.setPaint(new TexturePaint(source, new Rectangle(0, 0, 1, HEIGHT)));
	      ns = System.nanoTime();
	      for (int i = 0; i < TIMES; i++) {
	         destg.fillRect(0, 0, WIDTH, HEIGHT);
	      }
	      t = System.nanoTime() - ns;
	      System.out.printf("Full Texture Paint  in %.3fns%n", t / (double) TIMES);

	      destg.dispose();
	   }
	
	/**
	 * @param args
	 */
	public static void main2(String[] args)
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
