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

    public static final String UPDATE_COMMAND = "u", DEBUG_COMMAND = "d", EXECUTE_COMMAND = "e", PING_COMMAND = "p", ZERO_COMMAND = "z", CONFIG_COMMAND = "c", GET_CONFIG_COMMAND = "g";

    private static final int HISTORY_SIZE = 20;

    private String response;
    private String delayedLine;
    private boolean isOffering;
    private LinkedList<String> history = new LinkedList<>();
    private int speed;
    private int angle;

    private Timer timer = new Timer(true);
    private TimerTask pinger = null;
    private StatusReceiver statusReceiver = new StatusReceiver();
    private UpdateSender updateSender = new UpdateSender(this);
    private LineReceiver lineReceiver = new LineReceiver();

    public RobotConnection() {

    }

    public void setPingRate(int rateInMs) {

        if (pinger != null) {
            pinger.cancel();
        }

        pinger = new TimerTask() {
            @Override
            public void run() {
                ping();
            }
        };
        timer.schedule(pinger, rateInMs, rateInMs);
    }

    public List<String> getHistory() {
        return new ImmutableObservableList<>(history.toArray(new String[history.size()]));
    }

    synchronized private void ping() {

        try {
            sendLine(PING_COMMAND);
        } catch (PortConnectionException e) {
            e.printStackTrace();
        }

    }

    public String ask(String command, String message) throws PortConnectionException {
        synchronized (this) {
            sendLine(command + (message != null ? message : ""));
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

        speed = Integer.parseInt(data[0]);
        angle = Integer.parseInt(data[1]);
        if (statusReceiver != null)
            statusReceiver.update(speed, angle);
    }

    public void disconnect() {
        if (pinger != null)
            pinger.cancel();
        internalDisconnect();
    }

    public void lineReceived(String line) {
        System.err.println(line);
        history.addLast(line);
        if (history.size() > HISTORY_SIZE) {
            history.removeFirst();
        }

        if (lineReceiver!=null){
            lineReceiver.update(line);
        }
        if (line.startsWith(">")) {
            parseState(line);
        } else if (line.startsWith("OK")) {
            synchronized (this) {
                response = line;
                this.notify();
            }
        }

    }

    public void offerLine(String line) {
        delayedLine = line;
        if (!isOffering) {
            Thread thread = new Thread() {
                @Override
                public void run() {
                    try {
                        sendLine(delayedLine);
                        isOffering = false;
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            };
            thread.start();
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

    public LineReceiver getLineReceiver() {
        return lineReceiver;
    }
}
