package jsrl.gui;

import java.util.Comparator;
import java.util.List;
import java.util.Vector;

import javax.swing.event.TableModelListener;
import javax.swing.table.AbstractTableModel;

import jsrl.sys.ProcessInfo;
import jsrl.sys.ProcessList;
import jsrl.sys.ProcessListener;

public class ProcessTableModel extends AbstractTableModel implements ProcessListener
{

	protected class ProcSorter<T extends ProcessInfo> implements Comparator<T>
	{
		public int columnIndex = 3;
		
		public int compare(T p1, T p2)
		{
			if ((p1 == null) || (p2 == null))
				return 0;

			
			switch (columnIndex)
			{
			case (0):
				return p2.name().compareTo(p1.name());
			case (1):
				return p2.id() - p1.id();
			case (2):
				return p2.parentId() - p1.parentId();
			case (3):
				return (int)(p2.cpu().load() - p1.cpu().load());
			case (4):
				return p2.memory().used() - p1.memory().used();
			case (5):
				return p2.threads() - p1.threads();
			case (6):
				return p2.handles() - p1.handles();
			case (7):
				return p2.owner().compareTo(p1.owner());
			}
			return 0;
		}
		
	}
	
	protected ProcessList procMan = null;
	protected String columns[] = {"name", "pid", "parent", "cpu", "memory", "threads", "handles", "owner" };
	protected List<ProcessInfo> proc_list = new Vector<ProcessInfo>();
	//protected ProcSorter sorter = new ProcSorter<ProcessInfo>();
	protected int _columnIndex = 3;
	protected int proc_count = 0;
	
	public ProcessTableModel()
	{
		procMan = ProcessList.getInstance();
		procMan.addListener(this);
		update();
	}
	
	public void sortByColumn(int columnIndex)
	{
		_columnIndex = columnIndex;
	}
	
	public void update()
	{
		procMan.update();
		
		//proc_list = (ProcessInfo[])procMan.getList().values().toArray(proc_list);

		//this.fireTableDataChanged();
//		if (_columnIndex >= 0)
//		{
//			sorter.columnIndex = _columnIndex;
//			Arrays.sort(proc_list, sorter);
//		}
	}
	
	public ProcessInfo getProcessInfo(int index)
	{
		return proc_list.get(index);
	}
	
	
	public int getRowCount()
	{
		return proc_list.size();
	}

	public int getColumnCount()
	{
		return columns.length;
	}

	public String getColumnName(int columnIndex)
	{
		return columns[columnIndex];
	}

	public Class<?> getColumnClass(int columnIndex)
	{
		switch (columnIndex)
		{
		case (0):
			return String.class;
		case (1):
			return Integer.class;
		case (2):
			return Integer.class;
		case (3):
			return Integer.class;
		case (4):
			return Float.class;
		case (5):
			return Integer.class;
		case (6):
			return Integer.class;
		case (7):
			return String.class;
		}
		return String.class;
	}

	public boolean isCellEditable(int rowIndex, int columnIndex)
	{
		return false;
	}

	public Object getValueAt(int rowIndex, int columnIndex)
	{
	
		if (rowIndex >= proc_list.size())
			return null;
		
		ProcessInfo proc_info = proc_list.get(rowIndex);
		switch (columnIndex)
		{
		case (0):
			return proc_info.name();
		case (1):
			return proc_info.id();
		case (2):
			return proc_info.parentId();
		case (3):
			return (int)proc_info.cpu().load();
		case (4):
			return (float)proc_info.memory().used()/1024.0f;
		case (5):
			return proc_info.threads();
		case (6):
			return proc_info.handles();
		case (7):
			return proc_info.owner();
		}
		
		return null;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		// TODO Auto-generated method stub

	}

	public void dead_process(ProcessInfo proc) {
		// TODO Auto-generated method stub
		proc_list.remove(proc);
		this.fireTableDataChanged();
	}

	public void new_process(ProcessInfo proc) {
		// TODO Auto-generated method stub
		proc_list.add(proc);
		this.fireTableDataChanged();
	}

}
