package jsrl.test;

import java.util.*;
import java.net.SocketException;
import jsrl.net.*;


public class PingTest
{
    private static int seq;
    public PingTest()
    {

    }

    private static synchronized int nextSequenceNumber()
    {
        int retval = seq;
        seq++;
        
        if (seq == Integer.MAX_VALUE)
        {
            seq = 0;

        }

       

        return retval;

    }

   

    private static TimerTask makePingTask(final String addr)

    {

        TimerTask tt = new TimerTask()

        {

            public void run()

            {

                JavaPing jp = null;

       

                try

                {

                    jp = JavaPing.getInstance();

                }

                catch(Exception e)

                {

                    e.printStackTrace();

                }

      

                int mySeq = nextSequenceNumber();

               

                System.err.println("pinging " + addr + " with " + mySeq);

               

                jp.ping(addr, mySeq, 32);

            }

        };

       

        return tt;

    }

   

    public static void main(String[] args)

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

            public void ping_result(PingResult pingresult)

            {

                StringBuffer sb = new StringBuffer("ping_result:\n");

               

                sb.append("\tfrom: " + pingresult.from + "\n");

                sb.append("\tbytes: " + pingresult.bytes + "\n");

                sb.append("\ttime: " + pingresult.rtt + "\n");

                sb.append("\tttl: " + pingresult.ttl + "\n");

                sb.append("\tseq: " + pingresult.seq);

               

                System.err.println(sb.toString());

            }

        };

       
        try
        {
            jp.startListening(pl);            
        }
        catch (SocketException e)
        {
            return;
        }

        Timer timer = new Timer();

       

        timer.scheduleAtFixedRate(makePingTask("127.0.0.1"), 0, 1000);

        timer.scheduleAtFixedRate(makePingTask("64.233.187.99"), 0, 1000);

        // timer.scheduleAtFixedRate(makePingTask("65.214.39.7"), 0, 5000);

        timer.scheduleAtFixedRate(makePingTask("209.202.248.101"), 0, 10000);

    }

}