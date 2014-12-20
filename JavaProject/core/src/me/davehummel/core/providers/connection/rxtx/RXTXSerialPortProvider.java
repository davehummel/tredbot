package me.davehummel.core.providers.connection.rxtx;

import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.robot.RobotConnection;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

/**
 * Created by davidhummel on 12/18/14.
 */
public class RXTXSerialPortProvider implements SerialPortProvider {

    @Override
    public List<String> getSerialPortNames() {
        Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();
        List<String> ports = new ArrayList<String>();
        while (portEnum.hasMoreElements()){
            ports.add(((CommPortIdentifier) portEnum.nextElement()).getName());
        }
        return ports;
    }

    @Override
    public RobotConnection connectToPort(String portName) {
        Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();
        List<String> ports = new ArrayList<String>();
        while (portEnum.hasMoreElements()){
            ports.add(((CommPortIdentifier) portEnum.nextElement()).getName());
        }
        SerialPort serialPort = (SerialPort) portId.open(this.getClass().getName(),
                TIME_OUT);
        return null;
    }
}
