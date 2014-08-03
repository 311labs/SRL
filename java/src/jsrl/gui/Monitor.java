package jsrl.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.GradientPaint;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.geom.Line2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;
import java.util.Vector;

import javax.swing.JPanel;


public class Monitor extends JPanel implements Runnable
{
	private static final long serialVersionUID = 1L;
	public Thread thread;
	public long sleepAmount = 1000;
	protected int w, h;
	protected BufferedImage bimg;
	protected Graphics2D big;
	protected Font font = new Font("Times New Roman", Font.PLAIN, 11);
	protected Runtime r = Runtime.getRuntime();
	protected int columnInc;
	protected int pts[];
	protected int ptNum;
	protected int ascent, descent;
	protected Rectangle graphOutlineRect = new Rectangle();
	protected Rectangle2D mfRect = new Rectangle2D.Float();
	protected Rectangle2D muRect = new Rectangle2D.Float();
	protected Line2D graphLine = new Line2D.Float();
	protected Color graphColor = Color.DARK_GRAY;//new Color(46, 139, 87);
	protected Color bar_bg = Color.DARK_GRAY;//new Color(0, 100, 0);
	protected Color bar_color = new Color(15, 95, 255);
	
	protected String header, footer;
	protected int numLoadBars = 0;
	
	public boolean show_grid = true;
	
	public class Data
	{
		public List<Float> values = new LinkedList<Float>();
		public float max_value = 100.0f;
		public float percentage = 0.0f;
		public int limit = 100;
		public Color color = new Color(15, 95, 255);
		public boolean show_bar = true;
		public boolean show_plot = true;
		
		public void resize(int size)
		{
			// clear out old one if the new size is smaller then the old size
			while (values.size() > size)
			{
				values.remove(size);
			}
			
			limit = size;
		}
		
		
		public void add(float value)
		{
			if (values.size() >= limit)
			{
				values.remove(0);
			}

			percentage = (value/max_value) * 10.0f;
			values.add(values.size(), value);	
		}
	}
	
	protected List<Data> data_list = new Vector<Data>();
	
	public void newData(float maxValue)
	{
		Data dt = new Data();
		dt.max_value = maxValue;
		data_list.add(dt);
	}
	
	public void add(Data data)
	{
		data_list.add(data);
	}
	
	public Data get(int index)
	{
		return data_list.get(index);
	}

	public Monitor(String header, String footer)
	{
		this.header = header;
		this.footer = footer;
		setBackground(Color.black);
		//setForeground(arg0)
	}
	
	public Monitor()
	{
		setBackground(Color.black);
//		addMouseListener(new MouseAdapter()
//		{
//			public void mouseClicked(MouseEvent e)
//			{
//				if (thread == null)
//					start();
//				else
//					stop();
//			}
//		});
	}

	public void setHeader(String header)
	{
		this.header = header;
	}

	public void setFooter(String footer)
	{
		this.footer = footer;
	}
	
	public Dimension getMinimumSize()
	{
		return getPreferredSize();
	}

	public Dimension getMaximumSize()
	{
		return getPreferredSize();
	}

	public Dimension getPreferredSize()
	{
		return new Dimension(135, 80);
	}
	
	public void start()
	{
		thread = new Thread(this);
		thread.setPriority(Thread.MIN_PRIORITY);
		thread.setName("MemoryMonitor");
		thread.start();
	}

	public synchronized void stop()
	{
		thread = null;
		notify();
	}

	public void run()
	{

		Thread me = Thread.currentThread();

		while (thread == me && !isShowing() || getSize().width == 0)
		{
			try
			{
				Thread.sleep(sleepAmount);
			} catch (InterruptedException e)
			{
				return;
			}
		}

		while (thread == me && isShowing())
		{
			replot();
			try
			{
				Thread.sleep(sleepAmount);
			} catch (InterruptedException e)
			{
				break;
			}
			if (MonitorPanel.dateStampCB.isSelected())
			{
				System.out.println(new Date().toString());
			}
		}
		thread = null;
	}
	
	protected void update()
	{

	}
	
	
	public void paint(Graphics g)
	{
		if (big == null)
		{
			return;
		}

		bimg = (BufferedImage) createImage(w, h);
        big = bimg.createGraphics();
        grad_paint = new GradientPaint(0, 0, Color.DARK_GRAY,
                0, h, Color.BLACK);

        big.setPaint(grad_paint);
        big.fillRect(0, 0, w, h);
        //big.drawImage(bimg, 0, 0, w, h, null);		

		big.setFont(font);
		FontMetrics fm = big.getFontMetrics(font);
		ascent = (int) fm.getAscent() + 2;
		descent = (int) fm.getDescent() + 2;		
		
		//big.setBackground(getBackground());
		//big.clearRect(0, 0, w, h);		
		
		//big.drawImage(bimg, 0, 0, w, h, null);
		
		big.setColor(Color.white);
		
		numLoadBars = 0;
		for (Data data : data_list)
		{
			if (data.show_bar)
			{
				++numLoadBars;
		
			}	
		}
		// draw the header
		big.drawString(header, 4.0f, (float) ascent + 0.5f);
		// draw the footer
		big.drawString(footer, 30.0f * numLoadBars, h - descent);	
				
		// draw history graph
		if (show_grid)
			draw_graph();	
		
		draw_data();
		
		g.drawImage(bimg, 0, 0, this);	
		
	}
	
	GradientPaint grad_paint = null;

	public void replot()
	{
		Dimension d = getSize();
		if (d.width != w || d.height != h)
		{
			w = d.width;
			h = d.height;
			
			bimg = (BufferedImage) createImage(w, h);
            big = bimg.createGraphics();
            grad_paint = new GradientPaint(0, 0, Color.GRAY,
	                0, h, Color.BLACK);

            big.setPaint(grad_paint);
            big.fillRect(0, 0, w, h);
	        //big.drawImage(bimg, 0, 0, w, h, null);		

			big.setFont(font);
			FontMetrics fm = big.getFontMetrics(font);
			ascent = (int) fm.getAscent() + 2;
			descent = (int) fm.getDescent() + 2;
			//big.dispose();
		}
		update();
		repaint();
	}	
	
	void draw_graph()
	{
		big.setColor(graphColor);
		
		// determine graph boundries
		float font_margin = ascent + descent;
		float remainingHeight = (float) (h - (font_margin * 2) - 0.5f);
		int graphX = 30 * numLoadBars;
		int graphY = (int) font_margin;
		int graphW = w - graphX - 5;
		int graphH = (int) remainingHeight;
		
		
		// draw graph outline
		graphOutlineRect.setRect(graphX, graphY, graphW, graphH);
		big.draw(graphOutlineRect);
		
		// draw graph rows
		//int graphRow = graphH / 10;
		float rowHeight = remainingHeight / 10;
		
		for (float j = graphY; j <= graphH + graphY; j += rowHeight)
		{
			graphLine.setLine(graphX, j, graphX + graphW, j);
			big.draw(graphLine);
		}
		
		// draw scrolling columns
		int graphColumn = graphW / 15;

		if (columnInc == 0)
		{
			columnInc = graphColumn;
		}

		for (int j = graphX + columnInc; j < graphW + graphX; j += graphColumn)
		{
			graphLine.setLine(j, graphY, j, graphY + graphH);
			big.draw(graphLine);
		}

		--columnInc;

	}
	
	
	protected void draw_data()
	{
		// Calculate remaining size
		float bar_segments = 40.0f;
		float font_margin = ascent + descent;
		float remainingHeight = (float) (h - (font_margin * 2) - 0.5f);
		
		float blockHeight = remainingHeight / bar_segments;
		blockHeight -= 2.0f;
		
		while (blockHeight < 2.0f)
		{
			--bar_segments;
			blockHeight = remainingHeight / bar_segments;
			blockHeight -= 2.0f;			
		}
		
		float blockWidth = 20.0f;
		
		float xpos = 5.0f;
		float ypos = font_margin;
		
		int xscale = (int)(w - (30.0f*numLoadBars) - 5);
		
		for (Data data : data_list)
		{	
			ypos = font_margin;
			// rescal to width of plot area
			data.resize(xscale);
			if (data.show_bar)
			{
				// draw empty left hand side load bar
				int i = 0;
				// reverse load for painting
				int loaded_bars = (int)(data.percentage / 10.0f * bar_segments);
				int unloaded_bars = (int)bar_segments - loaded_bars;
				//int load = (int)(bar_segments - data.percentage);
				
				big.setColor(this.bar_bg);
				for (i=0; i < unloaded_bars; ++i)
				{
					mfRect.setRect(xpos, ypos, blockWidth, blockHeight);
					ypos += blockHeight + 2.0f;
					big.fill(mfRect);
				}
				
				big.setColor(data.color);
				for (i=0; i < loaded_bars; ++i)
				{
					mfRect.setRect(xpos, ypos, blockWidth, blockHeight);
					ypos += blockHeight + 2.0f;
					big.fill(mfRect);						
				}

				big.setColor(data.color);
				big.drawString(String.format("%.0f%%", data.percentage * 10.0f), xpos, h - descent);			

				xpos += blockWidth + 5;
			}
			// plot this set of data
			if (data.show_plot)
			{
				plot(data);
			}
		}
	}
	
	protected void plot(Data data)
	{
		big.setColor(data.color);
		
		float font_margin = ascent + descent;
		float xpos = 30.0f * numLoadBars;
		
		float yscale = (float) (h - (font_margin * 2) - 0.5f);
		
		// plot our data values (limit represents the graph drawing area)
		float xpos1 = (data.limit - data.values.size()) + (xpos);
		float ypos1 = font_margin + yscale;
		
		int count = 0;
		for (Float value : data.values)
		{
			if (value > data.max_value)
				value = data.max_value;
			float ypos2 = ((data.max_value - value)/data.max_value) * yscale;
			ypos2 += font_margin;
			//if (value > data.max_value)
			//System.out.println(ypos2 + " : " + yscale);
			if (count > 0)
			{
				graphLine.setLine(xpos1, ypos1, xpos1+1, ypos2);
				big.draw(graphLine);
			}
			++count;
			++xpos1;
			ypos1 = ypos2;
		}

	}
}
