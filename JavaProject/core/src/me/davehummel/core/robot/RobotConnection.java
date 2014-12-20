package me.davehummel.core.robot;

import java.io.*;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by davidhummel on 12/16/14.
 */
public abstract class RobotConnection {

    public static final String STATUS_COMMAND = "s",UPDATE_COMMAND = "u", DEBUG_COMMAND = "d", EXECUTE_COMMAND = "e", PING_COMMAND = "p",CONFIG_COMMAND = "c", GET_CONFIG_COMMAND = "g";
    private static final String CONNECTION_ERROR = "Connection Lost";
    private final BufferedReader input;
    private final PrintStream output;
    private String lastLineInput;


    public RobotConnection(InputStream input, OutputStream output) throws UnsupportedEncodingException {
        this.input = new BufferedReader(new InputStreamReader(input, "UTF-8"));
        this.output = new PrintStream(output);
    }

    public String getLastInput(){
        return lastLineInput;
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
        return lastLineInput;
    }

    synchronized protected void notifyNewInput(){
        try {
            lastLineInput = input.readLine();
        } catch (IOException e) {
            lastLineInput = CONNECTION_ERROR;
        }
        this.notify();
    }

    abstract public void disconnect();

}
