package jsrl.util;

public enum WeightUnits
{
    ShortTon(907.18474, true, false), Pound(2.204622622, false, true),
    Ounce(35.2739619, false, true), LongTon(1016.0469088, true, false),
    MetricTon(1000.0, true, false), Kilogram(1.0, true, false), 
    Gram(1000.0, false, true);
    
    private double base;
    private boolean isToLinear;
    private boolean isFromLinear;
    
    WeightUnits(double base, boolean isToLinear, boolean isFromLinear)
    {
        this.base = base;
        this.isToLinear = isToLinear;
        this.isFromLinear = isFromLinear;
    }
    
    private double linearForm(double weight)
    {
        return this.base * weight;
    }
    
    private double invLinearForm(double weight)
    {
        return weight / this.base;
    }
    
    private double getToBaseValue(double weight)
    {
        if (this.isToLinear)
            return linearForm(weight);
        return invLinearForm(weight);
    }
    
    private double getFromBaseValue(double weight)
    {
        if (this.isFromLinear)
            return linearForm(weight);
        return invLinearForm(weight);
    }
    
    double convertTo(double weight, WeightUnits unit)
    {
        return this.getFromBaseValue(unit.getToBaseValue(weight));
    } 
}
