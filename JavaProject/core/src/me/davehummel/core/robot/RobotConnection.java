package me.davehummel.core.robot;

import java.io.*;

/**
 * Created by davidhummel on 12/16/14.
 */
public class RobotConnection {

    public static final String STATUS_COMMAND = "s",UPDATE_COMMAND = "u", DEBUG_COMMAND = "d", EXECUTE_COMMAND = "e", PING_COMMAND = "p",CONFIG_COMMAND = "c", GET_CONFIG_COMMAND = "g";
    private final BufferedReader input;
    private final PrintStream output;

    private

    public RobotConnection(InputStream input, OutputStream output) throws UnsupportedEncodingException {
        this.input = new BufferedReader(new InputStreamReader(input, "UTF-8"));
        this.output = new PrintStream(output);
    }

    public String getLastInput(){
        return null;
    }

    public String ask(String command,String message) throws IOException {

        output.print(command+message);
        return getNextInput(1000);
    }

    public void notifyNewInput(){
        // TODO code to read into buffer from input

    }

}
