package me.davehummel.core.platforms.windows.dof;

/**
 * Created by Dave on 12/31/2014.
 */
public class DOFGroup {
    public final float mx,my,mz,ax,ay,az,gx,gy,gz,t;


    public DOFGroup(float mx, float my, float mz, float ax, float ay, float az, float gx, float gy, float gz, float t) {
        this.mx = mx;
        this.my = my;
        this.mz = mz;
        this.ax = ax;
        this.ay = ay;
        this.az = az;
        this.gx = gx;
        this.gy = gy;
        this.gz = gz;
        this.t = t;
    }
}
