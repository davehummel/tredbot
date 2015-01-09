package me.davehummel.core.robot;

/**
 * Created by Dave on 12/21/2014.
 */
public class StatusReceiver {
    public interface StatusReceiverListener {
        abstract public void onStatusUpdate(int speed, int angle);
    }

    private StatusReceiverListener listener = null;

    synchronized public void setListener(StatusReceiverListener listener){
        this.listener = listener;
    }

    synchronized void update(int speed, int angle){
        if (listener!=null)
            listener.onStatusUpdate(speed,angle);
    }
}
