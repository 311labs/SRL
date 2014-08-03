package jsrl.test;


import java.util.*;

import jsrl.net.*;
import jsrl.net.PingListener;
import java.net.SocketException;

public class MultiPinger
{
    private static volatile int seq;
    private static volatile HashMap pingResponses;
   
    static
    {
        pingResponses = new HashMap();
    }
    
    private static synchronized int nextSequenceNumber()
    {
        int retval = seq;
        
        seq++;
       
        if (seq == 65535)
        {
            seq = 0;
        }
        
        return retval;
    }
 
    private static void sendPing(String addr, int seq)
    {
        JavaPing jp = null;
        
        try
        {
            jp = JavaPing.getInstance();
        }
        catch(Exception e)
        {
            e.printStackTrace();
            System.exit(0);
        }

        jp.ping(addr, seq, 32);
    }
    
    private static int doPing(String addr, long millis)
    {
                    
        int retval = -1;
        
        PingPair pp = new PingPair();
        pp.lock = new Object();
        pp.rtt = -1;
        
        int nextSeq = nextSequenceNumber();
        Integer seqObj = new Integer(nextSeq);
        
        System.err.println("->pinging " + addr + " (" + nextSeq + ")");
        
        synchronized(pp.lock)
        {
            synchronized(pingResponses)
            {
                pingResponses.put(seqObj, pp);
            }            
            
            sendPing(addr, nextSeq);
            
            try
            {
                pp.lock.wait(millis);
            }
            catch(Exception e)
            {
                // IGNORE
            }
       
            retval = pp.rtt;
      
            synchronized(pingResponses)
            {
                pingResponses.remove(seqObj);
            }
        }
        
        return retval;
    }
    
    private static void buildThread(final String addr, final long millis)
    {
        Runnable r = new Runnable()
        {
            public void run()
            {
                while(true)
                {
                    int pingTime = doPing(addr, 5000);
                    
                    delay(millis);
                }
            }
        };
        
        Thread t = new Thread(r);
        t.start();
    }

   
    private static void initPing() throws SocketException
    {
        JavaPing jp = null;
        
        try
        {
            jp = JavaPing.getInstance();
        }
        catch(Exception e)
        {
            e.printStackTrace();
            System.exit(0);
        }
       
        PingListener pl = new PingListener()
        {
            public void ping_result(PingResult pingResult)
            {
                System.err.println("<- ping_result for sequence " + pingResult.seq + "...");
                System.err.println("\tfrom=" + pingResult.from + "\ttype=" + pingResult.type + "\trtt=" + pingResult.rtt);
                
                int result = -1;
                
                if (pingResult.type == JavaPing.ECHO_REPLY)
                {
                    result = Math.round(pingResult.rtt);
                }
                else if (pingResult.type == JavaPing.DESTINATION_UNREACHABLE)
                {
                    System.err.println("\tDESTINATION NOT REACHABLE");
                }
                else
                {
                    System.err.println("\t!!! UNKNOWN PACKET TYPE !!!");
                }
                        
                Integer resultObj = new Integer(result);
                Integer seqObj = new Integer(pingResult.seq);
              
                PingPair pp = null;
               
                synchronized(pingResponses)
                {
                    pp = (PingPair) pingResponses.get(seqObj);
                }
                
                if (pp != null)
                {
                    synchronized(pp.lock)
                    {
                        pp.rtt = result;
                        pp.lock.notify();
                    }
                }
                else
                {
                    System.err.println("\t!!! DUPLICATE RESPONSE FOR REQUEST !!!");
                }
            }
        };
        
        jp.startListening(pl);
        
        delay(5000);
    }
    
    private static void delay(long millis)
    {
        try
        {
            Thread.sleep(millis);
        }
        catch(Exception e)
        {
            // IGNORE
        }
    }
    
    public static void main(String[] args)
    {
        try
        {
            initPing();
            buildThread("127.0.0.1", 1000);
            buildThread("64.233.187.99", 2000);
            buildThread("65.214.39.7", 3000);
            buildThread("209.202.248.101", 4000);            
        }
        catch (SocketException e)
        {
            
        }
        
        // delay(5000);
        // buildThread("64.236.16.20", 5000);
    }
    
    private static class PingPair
    {
        public Object lock; 
        public int rtt;
    }
}
