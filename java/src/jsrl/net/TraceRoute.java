package jsrl.net;

import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

/**
 * TraceRoute is meant to be a static instance of a traceroute utility class
 * that will allow you to perform multiple traceroutes
 *
 * @author IAN STARNES;
 * @version $Rev$
 */
public final class TraceRoute implements Runnable
{
    public static class TraceRouteInfo 
    {
        /**
         * Inner Class that represents a hop in the trace route.
         * Each hop may have multiple attempts to calculate the
         * round trip time
         * @author ians
         *
         */
        public static class Hop
        {        
            protected String hopIP;
            protected List<Float> attempts = new Vector<Float>();
            protected float avgRTT = 0.0f;
            
            public Hop(String hopIP)
            {
                this.hopIP = hopIP;          
            }
            
            /** used internally to add a new result for an attempt at this hop in the trace */
            protected void addAttempt(float rtt, String ip)
            {
                this.hopIP = ip;
                attempts.add(rtt);
                float total = 0.0f;
                for (Float lrtt : attempts)
                {
                    total += lrtt;
                }
                avgRTT = total / attempts.size();
                
            }
            
            /** returns the calculated average round trip time */
            public float getAverageRTT()
            {
                return avgRTT;
            }
            
            /** how many attempts has the trace tried on this hop */
            public int numberOfAttempts()
            {
                return attempts.size();
            }
            
            /**
             * Returns the round trip time for the specified attempt
             * @param attempt
             * @return
             */
            public float getRTT(int attempt)
            {
                return attempts.get(attempt);
            }
            
            public float getLastRTT()
            {
                return attempts.get(attempts.size()-1);
            }
            
            public String getIP()
            {
                return hopIP;
            }
            
        }
        
        protected String host;
        protected String ip;
        protected List<Hop> hops = new Vector<Hop>();
        protected int attemptsPerHop = 1;
        public long start_time = System.currentTimeMillis();
        public long end_time = 0;
        
        public TraceRouteInfo(String host, int attemptsPerHop) throws UnknownHostException
        {
            this.host = host;
            this.ip =  java.net.InetAddress.getByName(host).getHostAddress();
            this.attemptsPerHop = attemptsPerHop;
        }
        
        public String getIP()
        {
            return this.ip;
        }
        
        public int numberOfHops()
        {
            return hops.size();
        }
        
        public List<Hop> getHops()
        {
            return hops;
        }
        
        public Hop getLastHop()
        {
            if (hops.size() > 0)
                return hops.get(hops.size()-1);
            return null;
        }
        
        public Hop getHop(int hop)
        {
            return hops.get(hop);
        }
        
        /**
         * returns the length of time in millis it has taken for the trace to complete
         * @return
         */
        public long getTraceTime()
        {
            if (end_time != 0)
                return end_time - start_time;
            return System.currentTimeMillis() - start_time;
        }
        
        protected boolean hasHopCompleted()
        {
            Hop hop = getLastHop();
            if (hop != null)
                return hop.attempts.size() >= attemptsPerHop;
            return false;
        }
        
        protected boolean hasTraceCompleted()
        {
            Hop hop = getLastHop();
            if (hop == null)
                return false;
            // use end time as a flag to not repeat logic
            if (end_time == 0)
            {
                if ((ip.compareTo(hop.hopIP) == 0) && (hasHopCompleted()))
                {
                    end_time = System.currentTimeMillis();
                    return true;
                }
                return false;
            }
            return true;
        }        
        
        protected Hop getNextHop()
        {
            Hop hop = getLastHop();
            if ((hop == null) || (hop.attempts.size() >= attemptsPerHop))
            {
                hop = new Hop("unknown");
                hops.add(hop);
            }
            return hop;
        }
    }    
    
    
    // local listener
    protected PingListener _listener = null;
    protected boolean _is_running = false;
    private static TraceRoute __singleton = null;
    
    static
    {
        try
        {
            __singleton = new TraceRoute();
        }
        catch(UnsatisfiedLinkError e)
        {
            System.err.println("Could not load native code for library: JSRL");
            System.exit(1);
        }
    }
    
    
    protected Map<String, TraceRouteInfo> traces = new HashMap<String, TraceRouteInfo>();
    protected List<TraceRouteListener> listeners = new Vector<TraceRouteListener>();
    protected boolean is_running = false;
    
    /** returns an instance of the Java Ping Utility 
     * @throws SocketException */
    public static TraceRoute getInstance() throws SocketException
    {
        if (__singleton == null)
            throw new SocketException("permission denied attempting to create raw socket!");
        return __singleton;
    }    
    
    public TraceRoute() 
    {
        
    }
    
    public void addListener(TraceRouteListener listener)
    {
        listeners.add(listener);
    }
    
    /**
     * Start tracing the route to the specified IP with 3 attempts per hop
     * @param host
     * @return
     * @throws UnknownHostException 
     */
    public TraceRouteInfo startTrace(String host) throws UnknownHostException
    {
        return startTrace(host, 3);
    }
    
    /**
     * Start tracing the route to the specified IP and use the specifiend number 
     * of attempts per hop
     * @param host
     * @param attemptsPerHop
     * @return
     * @throws UnknownHostException 
     */
    public TraceRouteInfo startTrace(String host, int attemptsPerHop) throws UnknownHostException
    {
        TraceRouteInfo routeInfo = new TraceRouteInfo(host, attemptsPerHop);
        synchronized(this) 
        {
            traces.put(host, routeInfo);
            if (!is_running)
            {
                is_running = true;
                new Thread(this).start();
            }
        }
        
        return routeInfo;
    }
    
    public TraceRouteInfo getRouteInfo(String host)
    {
        return traces.get(host);
    }
    
    public void run()
    {
        JavaPing pingTool = null;
        try
        {
            pingTool = JavaPing.getInstance();
        }
        catch (SocketException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return;
        }
        
        List<TraceRouteInfo> active = new Vector<TraceRouteInfo>();
       
        // running traces
        while (is_running)
        {
            // FIXME oh man this can't be right... yuck
            // for thread safety we move the active traces into a new list 
            active.clear();
            synchronized(this) 
            {
                active.addAll(traces.values());
            }
            
            for (TraceRouteInfo info : active)
            {
                TraceRouteInfo.Hop hop = info.getNextHop();
                try
                {
                    PingResult result = pingTool.pingpong(info.ip, 
                                    info.numberOfHops()+hop.numberOfAttempts(), 16, 10000, info.numberOfHops());
                    hop.addAttempt(result.rtt, result.from);


                    
                }
                catch (SocketTimeoutException e)
                {
                    hop.addAttempt(-5.0f, "unknown");
                }
                
                for (TraceRouteListener listener : listeners)
                {
                    listener.traceHop(info);
                    
                    if (info.hasHopCompleted())
                        listener.traceHopComplete(info);
                    
                    if (info.hasTraceCompleted())
                        listener.traceComplete(info);
                }
                
                if (info.hasTraceCompleted())
                {
                    synchronized(this) 
                    {
                        traces.remove(info.host);
                    }
                }
            }
            
            if (traces.size() == 0)
            {
                synchronized(this) 
                {
                    is_running = false;
                }
            }
        }
    }
    
    public static void main(String[] args)
    {
        if (args.length == 0)
        {
            System.out.println("please provide a host to trace");
            return;
        }
        
        String host = args[0];
        
        class TraceTestListener implements TraceRouteListener
        {
            public void traceHop(TraceRouteInfo info)
            {
                if (info.getLastHop().numberOfAttempts() == 1)
                    System.out.format("%d", info.numberOfHops());
                float rtt = info.getLastHop().getLastRTT();
                if (rtt < 0.0f)
                    System.out.print("\t*");
                else
                    System.out.format("\t%1.2f", rtt);
            }

            public void traceHopComplete(TraceRouteInfo info)
            {
                System.out.format("\t%s\n", info.getLastHop().getIP());
            }

            public void traceComplete(TraceRouteInfo info)
            {
                System.out.format("trace completed in %d ms\n", info.getTraceTime());
            }
        }
        
        TraceTestListener listener = new TraceTestListener();
        TraceRoute tracer = new TraceRoute();
        tracer.addListener(listener);
        try
        {
            tracer.startTrace(host);
            System.out.format("tracing route to %s(%s):\n", host, tracer.getRouteInfo(host).getIP());
        }
        catch (UnknownHostException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
         
        
    }
    
}
