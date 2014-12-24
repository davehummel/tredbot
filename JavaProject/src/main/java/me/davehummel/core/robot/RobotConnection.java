package me.davehummel.core.robot;

import com.sun.javafx.collections.ImmutableObservableList;
import me.davehummel.core.providers.connection.PortConnectionException;

import java.util.LinkedList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by davidhummel on 12/16/14.
 */
public abstract class RobotConnection {

    public static final String STATUS_COMMAND = "s",UPDATE_COMMAND = "u", DEBUG_COMMAND = "d", EXECUTE_COMMAND = "e", PING_COMMAND = "p",CONFIG_COMMAND = "c", GET_CONFIG_COMMAND = "g";

    private static final int HISTORY_SIZE = 20;

    private String response;
    private LinkedList<String> history = new LinkedList<>();
    private int speed;
    private int turnRemaining;
    private boolean turnLeft;

    private Timer timer = new Timer(true);
    private TimerTask pinger = null;
    private StatusReceiver statusReceiver = new StatusReceiver();
    private UpdateSender updateSender = new UpdateSender(this);

    public RobotConnection() {

    }

    public void setPingRate(int rateInMs){

        if (pinger!=null){
            pinger.cancel();
        }

        pinger = new TimerTask() {
            @Override
            public void run() {
                    ping();
            }
        };
        timer.schedule(pinger,rateInMs,rateInMs);
    }

    public List<String> getHistory(){
        return new ImmutableObservableList<>(history.toArray(new String[history.size()]));
    }

    synchronized private void ping(){

        try {
            sendLine(PING_COMMAND);
        } catch (PortConnectionException e) {
            e.printStackTrace();
        }

    }

    public String ask(String command,String message) throws PortConnectionException {
        synchronized (this) {
            sendLine (command + (message!=null? message:""));
            return waitNextLine(1000);
        }
    }


    private String waitNextLine(int timeoutMS) {
        try {
            this.wait(timeoutMS);
        } catch (InterruptedException e) {

        }
        return response;
    }

    private void parseState(String state) {
        String[] data = state.substring(1).split(":");
        turnLeft = "L".equals(data[0]);
        turnRemaining = Integer.parseInt(data[1]);
        speed = Integer.parseInt(data[2]);
        if (statusReceiver != null)
            statusReceiver.update(speed,turnRemaining,turnLeft);
    }

    public int getSpeed(){
        return speed;
    }

    public int getTurnRemaining(){
        return turnRemaining;
    }

    public boolean isTurnLeft(){
        return turnLeft;
    }

    public void disconnect(){
        if (pinger!=null)
            pinger.cancel();
        internalDisconnect();
    }

    public void lineReceived( String line){
        System.err.println(line);
        history.addLast(line);
        if (history.size()>HISTORY_SIZE){
            history.removeFirst();
        }
        if (line.startsWith(">")){
            parseState(line);
        }else if (line.startsWith("OK")){
            synchronized (this) {
                response = line;
                this.notify();
            }
        }

    }

    abstract protected void internalDisconnect();

    abstract protected void sendLine(String line) throws PortConnectionException;


    public UpdateSender getUpdateSender() {
        return updateSender;
    }

    public StatusReceiver getStatusReceiver() {
        return statusReceiver;
    }
}
