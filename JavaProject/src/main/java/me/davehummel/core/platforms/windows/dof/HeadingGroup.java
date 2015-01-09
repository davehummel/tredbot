package me.davehummel.core.platforms.windows.dof;

/**
 * Created by Dave on 12/31/2014.
 */
public class HeadingGroup {
    public static final HeadingGroup ZERO = new HeadingGroup(0,0,0);
    public final float x,y,z;


    public HeadingGroup(float x, float y, float z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
}
