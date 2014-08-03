package jsrl.util;

public enum TemperatureUnits
{
    Fahrenheit(1.8, 32.0, false, true), 
    Celsius(1.0, 0.0, true, false), 
    Kelvin(1.0, -273.15, true, false);
    
    private double base;
    private double offset;
    private boolean isToLinear;
    private boolean isFromLinear;
    
    TemperatureUnits(double base, double offset, 
                    boolean isToLinear, boolean isFromLinear)
    {
        this.base = base;
        this.offset = offset;
        this.isToLinear = isToLinear;
        this.isFromLinear = isFromLinear;
    }
    
    private double linearForm(double temperature)
    {
        return (this.base * temperature) + this.offset;
    }
    
    private double invLinearForm(double temperature)
    {
        return (temperature - this.offset) / this.base;
    }
    
    private double getToBaseValue(double temperature)
    {
        if (this.isToLinear)
            return linearForm(temperature);
        return invLinearForm(temperature);
    }
    
    private double getFromBaseValue(double temperature)
    {
        if (this.isFromLinear)
            return linearForm(temperature);
        return invLinearForm(temperature);
    }
    
    double convertTo(double temperature, TemperatureUnits unit)
    {
        return this.getFromBaseValue(unit.getToBaseValue(temperature));
    } 
    
}
