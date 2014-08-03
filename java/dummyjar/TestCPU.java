

import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

import javax.swing.*;

/**
 * Created by IntelliJ IDEA.
 * User: jordia
 * Date: Feb 23, 2004
 * Time: 6:26:35 PM
 * To change this template use Options | File Templates.
 */
public class TestCPU
{
    public static void main(String[] args)
    {
//		try
//		{
//			Enumeration<NetworkInterface> all = NetworkInterface.getNetworkInterfaces();
//			while (all.hasMoreElements())
//			{
//				NetworkInterface iface = all.nextElement();
//				System.out.println(iface.getName() + "    " + iface.getDisplayName());
//			}
//		
//		}
//		catch (SocketException e)
//		{
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
        System.err.println("Eating CPU...");
        
        while ( true )
        {
           int x =0;
           int y = 1;
           int z = 2;
        }
    }
}
