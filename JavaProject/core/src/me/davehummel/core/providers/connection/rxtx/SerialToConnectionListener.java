package me.davehummel.core.providers.connection.rxtx;

import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import me.davehummel.core.robot.RobotConnection;

/**
 * Created by Dave on 12/20/2014.
 */
public class SerialToConnectionListener implements SerialPortEventListener {

    private final RobotConnection connection;

    public SerialToConnectionListener(RobotConnection connection) {
        this.connection = connection;
    }

    @Override
    public void serialEvent(SerialPortEvent serialPortEvent) {
        if (serialPortEvent.getEventType() ==SerialPortEvent.DATA_AVAILABLE) {
            connection.notifyNewInput();
        }
    }
}
