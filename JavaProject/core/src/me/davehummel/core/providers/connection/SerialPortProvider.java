package me.davehummel.core.providers.connection;

import me.davehummel.core.robot.RobotConnection;

import java.util.List;

/**
 * Created by davidhummel on 12/16/14.
 */
public interface SerialPortProvider {

    List<String> getSerialPortNames();

    RobotConnection connectToPort(String portName);

}
