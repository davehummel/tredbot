package me.davehummel.core.platforms.windows.dof;

import org.apache.commons.math3.stat.descriptive.SummaryStatistics;

/**
 * Created by Dave on 12/31/2014.
 */
public class DOFSensor {

    private DOFGroup current;
    private SummaryStatistics[] magZeroStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
    private SummaryStatistics[] accZeroStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
    private SummaryStatistics[] gyroZeroStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};

    private SummaryStatistics[] magAllStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
    private SummaryStatistics[] accAllStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
    private SummaryStatistics[] gyroAllStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};

    private float acclDeadZoneX = 0;
    private float acclDeadZoneY = 0;
    private float acclDeadZoneZ = 0;
    private float gyroDeadZoneX = 0;
    private float gyroDeadZoneY = 0;
    private float gyroDeadZoneZ = 0;

    private boolean isInitializing = true;
    private HeadingGroup magneticHeading;
    private HeadingGroup gyroHeading;

    public void update(int msDur, DOFGroup values){

        if (isInitializing){
            updateZero(values);
            current = values;
        }else {
            current = fix(values);
            updateAll(current);
            process(msDur, current);
        }
    }

    private DOFGroup fix(DOFGroup values) {
        float ax = zero(values.ax, (float) accZeroStats[0].getMean(),acclDeadZoneX);
        float ay = zero(values.ay, (float) accZeroStats[1].getMean(),acclDeadZoneY);
        float az = zero(values.az, (float) accZeroStats[2].getMean(),acclDeadZoneZ);

        float gx = zero(values.gx, (float) gyroZeroStats[0].getMean(),gyroDeadZoneX);
        float gy = zero(values.gy, (float) gyroZeroStats[1].getMean(),gyroDeadZoneY);
        float gz = zero(values.gz, (float) gyroZeroStats[2].getMean(),gyroDeadZoneZ);

        return new DOFGroup(values.mx,values.my,values.mz,ax,ay,az,gx,gy,gz,values.t);
    }

    private float zero(float in, float mean, float deadZone) {
        float out =  (in - mean);
        if (Math.abs(out)<deadZone/2)
            return 0;
        return out;
    }

    private void updateZero(DOFGroup values) {
        magZeroStats[0].addValue(values.mx);
        magZeroStats[1].addValue(values.my);
        magZeroStats[2].addValue(values.mz);
        accZeroStats[0].addValue(values.ax);
        accZeroStats[1].addValue(values.ay);
        accZeroStats[2].addValue(values.az-.980665);
        gyroZeroStats[0].addValue(values.gx);
        gyroZeroStats[1].addValue(values.gy);
        gyroZeroStats[2].addValue(values.gz);
    }
    
    private void updateAll(DOFGroup values) {
        magAllStats[0].addValue(values.mx);
        magAllStats[1].addValue(values.my);
        magAllStats[2].addValue(values.mz);
        accAllStats[0].addValue(values.ax);
        accAllStats[1].addValue(values.ay);
        accAllStats[2].addValue(values.az);
        gyroAllStats[0].addValue(values.gx);
        gyroAllStats[1].addValue(values.gy);
        gyroAllStats[2].addValue(values.gz);
    }

    private void process(int msDur, DOFGroup values) {
        if (gyroHeading == null){
            gyroHeading = new HeadingGroup(magneticHeading.x,magneticHeading.y,magneticHeading.z);
        }else{
            gyroHeading = new HeadingGroup(gyroHeading.x+(float)(values.gx*msDur/1000.0),gyroHeading.y+(float)(values.gy*msDur/1000.0),gyroHeading.z+(float)(values.gz*msDur/1000.0));
        }
    }

    public void ready(){
        if (isInitializing) {
            isInitializing = false;
        } else {
             isInitializing = true;
             magZeroStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
             accZeroStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
             gyroZeroStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
        }
    }
    
    public DOFGroup getZeroMins(){
        return new DOFGroup((float)magZeroStats[0].getMin(),(float)magZeroStats[1].getMin(),(float)magZeroStats[2].getMin(),
                (float)accZeroStats[0].getMin(),(float)accZeroStats[1].getMin(),(float)accZeroStats[2].getMin(),
                (float)gyroZeroStats[0].getMin(),(float)gyroZeroStats[1].getMin(),(float)gyroZeroStats[2].getMin(),0f);
    }
    public DOFGroup getZeroMaxs(){
        return new DOFGroup((float)magZeroStats[0].getMax(),(float)magZeroStats[1].getMax(),(float)magZeroStats[2].getMax(),
                (float)accZeroStats[0].getMax(),(float)accZeroStats[1].getMax(),(float)accZeroStats[2].getMax(),
                (float)gyroZeroStats[0].getMax(),(float)gyroZeroStats[1].getMax(),(float)gyroZeroStats[2].getMax(),0f);
    }
    public DOFGroup getZeroAvg(){
        return new DOFGroup((float)magZeroStats[0].getMean(),(float)magZeroStats[1].getMean(),(float)magZeroStats[2].getMean(),
                (float)accZeroStats[0].getMean(),(float)accZeroStats[1].getMean(),(float)accZeroStats[2].getMean(),
                (float)gyroZeroStats[0].getMean(),(float)gyroZeroStats[1].getMean(),(float)gyroZeroStats[2].getMean(),0f);
    }

    public DOFGroup getAllMins(){
        return new DOFGroup((float)magAllStats[0].getMin(),(float)magAllStats[1].getMin(),(float)magAllStats[2].getMin(),
                (float)accAllStats[0].getMin(),(float)accAllStats[1].getMin(),(float)accAllStats[2].getMin(),
                (float)gyroAllStats[0].getMin(),(float)gyroAllStats[1].getMin(),(float)gyroAllStats[2].getMin(),0f);
    }
    public DOFGroup getAllMaxs(){
        return new DOFGroup((float)magAllStats[0].getMax(),(float)magAllStats[1].getMax(),(float)magAllStats[2].getMax(),
                (float)accAllStats[0].getMax(),(float)accAllStats[1].getMax(),(float)accAllStats[2].getMax(),
                (float)gyroAllStats[0].getMax(),(float)gyroAllStats[1].getMax(),(float)gyroAllStats[2].getMax(),0f);
    }
    public DOFGroup getAllAvg(){
        return new DOFGroup((float)magAllStats[0].getMean(),(float)magAllStats[1].getMean(),(float)magAllStats[2].getMean(),
                (float)accAllStats[0].getMean(),(float)accAllStats[1].getMean(),(float)accAllStats[2].getMean(),
                (float)gyroAllStats[0].getMean(),(float)gyroAllStats[1].getMean(),(float)gyroAllStats[2].getMean(),0f);
    }

    public HeadingGroup getMagneticHeading(){
        return magneticHeading;
    }

    public void resetRunningStats(){
        magAllStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
        accAllStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
        gyroAllStats = new SummaryStatistics[]{new SummaryStatistics(),new SummaryStatistics(),new SummaryStatistics()};
    }

    public void revertGyroHeading(){
        gyroHeading = null;
    }

    public DOFGroup getCurrent() {
        return current;
    }

    public void setMagneticHeading(HeadingGroup magneticHeading) {
        this.magneticHeading = magneticHeading;
    }

    public HeadingGroup getGyroHeading(){
        if (gyroHeading == null)
            return HeadingGroup.ZERO;
        return gyroHeading;
    }

//    public void calcMagneticHeading(){
//        float z = (float) (Math.atan2(current.my, current.mx) * 180 / Math.PI);
//        if (z<0)
//            z+=360;
//        magneticHeading = new HeadingGroup(0,0,z);
//    }

    public void setAcclDeadZoneX(float acclDeadZone) {
        this.acclDeadZoneX = acclDeadZone;
    }
    public void setAcclDeadZoneY(float acclDeadZone) {
        this.acclDeadZoneY = acclDeadZone;
    }
    public void setAcclDeadZoneZ(float acclDeadZone) {
        this.acclDeadZoneZ = acclDeadZone;
    }

    public void setGyroDeadZoneX(float gyroDeadZone) {
        this.gyroDeadZoneX = gyroDeadZone;
    }
    public void setGyroDeadZoneY(float gyroDeadZone) {
        this.gyroDeadZoneY = gyroDeadZone;
    }
    public void setGyroDeadZoneZ(float gyroDeadZone) {
        this.gyroDeadZoneZ = gyroDeadZone;
    }
}
