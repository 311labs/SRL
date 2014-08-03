package jsrl.net;

import jsrl.net.TraceRoute.TraceRouteInfo;

/**
 * Used in conjuction with the TraceRoute class to receive call backs for when
 * a trace has completed
 *
 * @author IAN STARNES &lt;&gt;
 * @version $Rev$
 */
public interface TraceRouteListener 
{
    /** called when a trace hop event has been received */
    public void traceHop(TraceRouteInfo info);
    /** called when the trace is moving to the next hop */
    public void traceHopComplete(TraceRouteInfo info);
    /** called when the trace has completed */
    public void traceComplete(TraceRouteInfo info);
}
