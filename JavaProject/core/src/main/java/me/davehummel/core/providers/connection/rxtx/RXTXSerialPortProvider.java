package me.davehummel.core.providers.connection.rxtx;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.robot.RobotConnection;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.TooManyListenersException;

/**
 * Created by davidhummel on 12/18/14.
 */
public class RXTXSerialPortProvider implements SerialPortProvider {

    private static final int TIME_OUT =1000 ;

    @Override
    public List<String> getSerialPortNames() {
        Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

        List<String> ports = new ArrayList<String>();
        do {
            ports.add(((CommPortIdentifier) portEnum.nextElement()).getName());
        } while (portEnum.hasMoreElements());
        return ports;
    }

    @Override
    public RobotConnection connectToPort(String portName) throws PortConnectionException {
        Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();
        CommPortIdentifier portId = null;
        boolean found = false;
        while (portEnum.hasMoreElements()){
            portId = (CommPortIdentifier) portEnum.nextElement();

            if (portId.getName().equals(portName)){
                found = true;
                break;
            }
        }
        if (found == false) {
            throw new PortConnectionException("Port \"" + portName + "\" not found");
        }
        try {
            final SerialPort serialPort = (SerialPort) portId.open(this.getClass().getName(),
                    TIME_OUT);

            RobotConnection connection = new RobotConnection(serialPort.getInputStream(),serialPort.getOutputStream()) {
                @Override
                public void internalDisconnect() {
                    serialPort.removeEventListener();
                    serialPort.close();
                }
            };
            serialPort.addEventListener(new SerialToConnectionListener(connection));
            connection.setPingRate(TIME_OUT);
            return connection;
        } catch (PortInUseException e) {
            throw new PortConnectionException("Port \"" + portName + "\" is in use.");
        } catch (Exception e) {
            e.printStackTrace();
            throw new PortConnectionException("Port \"" + portName + "\" could not be used:"+e.getMessage());
        }

    }

    @Override
    public void shutdown() {

    }
}
