package jsrl.util;

public enum TimeUnits
{
    Years(31557600.0, true, false), Months(604800.0, true, false),
    Weeks(604800.0, true, false), Days(86400, true, false),
    Hours(3600.0, true, false), Minutes(60, true, false),
    Seconds(1.0, true, true), Milliseconds(1000.0, false, true),
    Microseconds(1000000.0, false, true), Nanoseconds(1000000000.0, false, true);
    
    
    private double base;
    private boolean isToLinear;
    private boolean isFromLinear;
    
    TimeUnits(double base, boolean isToLinear, boolean isFromLinear)
    {
        this.base = base;
        this.isToLinear = isToLinear;
        this.isFromLinear = isFromLinear;
    }
    
    private double linearForm(double time)
    {
        return this.base * time;
    }
    
    private double invLinearForm(double time)
    {
        return time / this.base;
    }
    
    private double getToBaseValue(double time)
    {
        if (this.isToLinear)
            return linearForm(time);
        return invLinearForm(time);
    }
    
    private double getFromBaseValue(double time)
    {
        if (this.isFromLinear)
            return linearForm(time);
        return invLinearForm(time);
    }
    
    double convertTo(double time, TimeUnits unit)
    {
        return this.getFromBaseValue(unit.getToBaseValue(time));
    } 
}
