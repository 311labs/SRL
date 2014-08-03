package jsrl.gui;

import java.util.Arrays;
import java.util.Comparator;

import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;

import jsrl.sys.ProcessInfo;
import jsrl.sys.ProcessList;

public class ProcessTableModel implements TableModel
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
	protected ProcessInfo[] proc_list = new ProcessInfo[1];
	protected ProcSorter sorter = new ProcSorter<ProcessInfo>();
	protected int _columnIndex = 3;
	protected int proc_count = 0;
	
	public ProcessTableModel()
	{
		procMan = ProcessList.getInstance();
		update();
	}
	
	public void sortByColumn(int columnIndex)
	{
		_columnIndex = columnIndex;
	}
	
	public void update()
	{
		procMan.update();
		
		proc_list = (ProcessInfo[])procMan.getList().values().toArray(proc_list);
		proc_count = procMan.getList().size();
		
		if (_columnIndex >= 0)
		{
			sorter.columnIndex = _columnIndex;
			Arrays.sort(proc_list, sorter);
		}
	}
	
	
	public int getRowCount()
	{
		return proc_count;
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
		return getValueAt(0, columnIndex).getClass();
	}

	public boolean isCellEditable(int rowIndex, int columnIndex)
	{
		return false;
	}

	public Object getValueAt(int rowIndex, int columnIndex)
	{
	
		if (rowIndex >= proc_count)
			return null;
		
		switch (columnIndex)
		{
		case (0):
			return proc_list[rowIndex].name();
		case (1):
			return proc_list[rowIndex].id();
		case (2):
			return proc_list[rowIndex].parentId();
		case (3):
			return (int)proc_list[rowIndex].cpu().load();
		case (4):
			return (float)proc_list[rowIndex].memory().used()/1024.0f;
		case (5):
			return proc_list[rowIndex].threads();
		case (6):
			return proc_list[rowIndex].handles();
		case (7):
			return proc_list[rowIndex].owner();
		}
		
		return null;
	}

	public void setValueAt(Object aValue, int rowIndex, int columnIndex)
	{
		// TODO Auto-generated method stub

	}

	public void addTableModelListener(TableModelListener l)
	{
		// TODO Auto-generated method stub

	}

	public void removeTableModelListener(TableModelListener l)
	{
		// TODO Auto-generated method stub

	}

	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		// TODO Auto-generated method stub

	}

}
