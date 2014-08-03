package jsrl.util;

public enum SpeedUnits
{
    Knots(1.94384449), MilesPerHour(2.237), MilesPerMinute(0.037282272),
    FeetPerMinute(196.850394), FeetPerSecond(3.2808399),
    KilometersPerHour(3.6), KilometersPerMinute(0.06), MetersPerSecond(1.0);
    
    private double base;
    
    SpeedUnits(double base)
    {
        this.base = base;
    }
    
    double convertTo(double speed, SpeedUnits unit)
    {
        // get the base value for our value unit
        double result = speed / unit.base;
        // now convert to our new unit
        return result * this.base;
    }
    
}
