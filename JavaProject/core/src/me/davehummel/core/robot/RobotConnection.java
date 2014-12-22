package me.davehummel.core.robot;

import java.io.*;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by davidhummel on 12/16/14.
 */
public abstract class RobotConnection {

    public static final String STATUS_COMMAND = "s",UPDATE_COMMAND = "u", DEBUG_COMMAND = "d", EXECUTE_COMMAND = "e", PING_COMMAND = "p",CONFIG_COMMAND = "c", GET_CONFIG_COMMAND = "g";
    private static final String CONNECTION_ERROR = "Connection Lost";
    private final BufferedReader input;
    private final PrintStream output;
    private String lastLineInput,response;
    private int speed;
    private int turnRemaining;
    private boolean turnLeft;
    private int pingRate;
    private Timer timer = new Timer(true);
    private TimerTask pinger = null;
    private StatusReceiver statusReceiver = new StatusReceiver();
    private UpdateSender updateSender = new UpdateSender(this);

    public RobotConnection(InputStream input, OutputStream output) {
        BufferedReader reader = null;
        try {
            reader = new BufferedReader(new InputStreamReader(input, "UTF-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        this.input = reader;
        this.output = new PrintStream(output);
    }

    public void setPingRate(int rateInMs){
        this.pingRate = rateInMs;

        if (pinger!=null){
            pinger.cancel();
        }

        pinger = new TimerTask() {
            @Override
            public void run() {
                    ping();
            }
        };
    }

    public String getLastInput(){
        return lastLineInput;
    }

    synchronized private void ping(){
        output.print(PING_COMMAND);
    }

    synchronized public String ask(String command,String message) throws IOException {
        output.print(command+message);
        return getNextInput(1000);
    }

    private String getNextInput(int timeoutMS) {
        try {
            this.wait(timeoutMS);
        } catch (InterruptedException e) {

        }
        return response;
    }

    synchronized public void notifyNewInput(){
        try {
            String temp = input.readLine();
            if (temp.startsWith(">")){
                parseState(temp);
            }else if (temp.startsWith("OK:")){
                response = temp;
                this.notify();
            }else{
                lastLineInput = temp;
            }

        } catch (IOException e) {
            lastLineInput = CONNECTION_ERROR;
            response = CONNECTION_ERROR;
        }
        this.notify();
    }

    private void parseState(String state) {
        String[] data = state.substring(1).split(":");
        turnLeft = "L"==data[0];
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


    abstract protected void internalDisconnect();


    public UpdateSender getUpdateSender() {
        return updateSender;
    }

    public StatusReceiver getStatusReceiver() {
        return statusReceiver;
    }
}
