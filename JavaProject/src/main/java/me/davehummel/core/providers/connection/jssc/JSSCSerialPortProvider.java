package me.davehummel.core.providers.connection.jssc;

import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortList;
import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.robot.RobotConnection;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by davidhummel on 12/18/14.
 */
public class JSSCSerialPortProvider implements SerialPortProvider {

    private static final int TIME_OUT =1000 ;

    @Override
    public List<String> getSerialPortNames() {
        String[] portNames = SerialPortList.getPortNames();

        List<String> ports = new ArrayList<String>();
        for (String name:portNames){
            ports.add(name);
        }
        return ports;
    }

    @Override
    public RobotConnection connectToPort(String portName) throws PortConnectionException {

            final SerialPort serialPort = new SerialPort(portName);
    try{
        serialPort.openPort();
        serialPort.setParams(57600,8,1,0);
    }catch (SerialPortException spe){

    }
            RobotConnection connection = new RobotConnection() {
                @Override
                public void internalDisconnect() {
                    try {
                        serialPort.removeEventListener();
                        serialPort.closePort();
                    } catch (SerialPortException e) {
                        e.printStackTrace();
                    }

                }

                @Override
                protected void sendLine(String line) throws PortConnectionException {
                    try {
                        System.out.println("Sending:"+line);
                        serialPort.writeString(line);
                    } catch (SerialPortException e) {
                        e.printStackTrace();
                        throw new PortConnectionException(e.getMessage());
                    }
                }
            };


        try {
            serialPort.addEventListener(new SerialToConnectionListener(connection,serialPort));
        } catch (SerialPortException e) {
            e.printStackTrace();
           throw new PortConnectionException(e.getMessage());
        }
        connection.setPingRate(TIME_OUT);
        return connection;

    }

    @Override
    public void shutdown() {

    }
}
