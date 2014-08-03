package jsrl.util;

public enum LengthUnits
{
    Mile(0.00062137119), NauticalMile(0.0005399568), Yard(1.093613298), Foot(3.280839895), 
    Inch(39.3700787402), Kilometer(0.001), Meter(1.0), Centimeter(100.0), Millimeter(1000.0);
    
    private double inMeters;
    
    LengthUnits(double meters)
    {
        this.inMeters = meters;
    }
    
    double convertTo(double length, LengthUnits lengthUnit)
    {
        // get the base value for our value unit
        double result = length / lengthUnit.inMeters;
        // now convert to our new unit
        return result * this.inMeters;
    }
}
