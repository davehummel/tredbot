package me.davehummel.core.providers.connection.jssc;


import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import me.davehummel.core.robot.RobotConnection;

/**
 * Created by Dave on 12/20/2014.
 */
public class SerialToConnectionListener implements SerialPortEventListener {

    private final RobotConnection connection;

    private final StringBuilder message = new StringBuilder();
    private final SerialPort serialPort;

    public SerialToConnectionListener(RobotConnection connection,SerialPort serialPort) {
        this.connection = connection;
        this.serialPort = serialPort;
    }

    @Override
    public void serialEvent(SerialPortEvent event) {
        if(event.isRXCHAR() && event.getEventValue() > 0){
            try {
                byte buffer[] = serialPort.readBytes();
                for (byte b: buffer) {
                    if ( (b == '\r' || b == '\n')) {
                        if (message.length() > 0) {
                            String toProcess = message.toString();
                            connection.lineReceived(toProcess);
                            message.setLength(0);
                        }
                    }
                    else {
                        message.append((char)b);
                    }
                }
            }
            catch (SerialPortException ex) {
                ex.printStackTrace();
                connection.disconnect();
            }
        }
    }
}
