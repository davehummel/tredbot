package me.davehummel.core.robot;

/**
 * Created by Dave on 12/31/2014.
 */
public class LineReceiver {
    public interface LineReceiverListener {
        abstract public void onLine(String line);
    }

    private LineReceiverListener listener = null;

    synchronized public void setListener(LineReceiverListener listener){
        this.listener = listener;
    }

    synchronized void update(String line){
        if (listener!=null)
            listener.onLine(line);
    }

}
