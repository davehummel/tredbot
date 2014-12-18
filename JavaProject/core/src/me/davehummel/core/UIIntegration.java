package me.davehummel.core;

import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.providers.localstore.LocalStoreProvider;
import me.davehummel.core.robot.RobotConnection;
import me.davehummel.core.robot.RobotSettings;

import java.util.List;

/**
 * Created by davidhummel on 12/16/14.
 */
public class UIIntegration {

    private SerialPortProvider spp;
    private LocalStoreProvider lsp;
    private RobotConnection connection;

    public UIIntegration(SerialPortProvider spp, LocalStoreProvider lsp) {
        this.spp = spp;
        this.lsp = lsp;
    }

    public void storeRobotSettings(RobotSettings robotSettings){
        robotSettings.writeToStore(lsp);
    }

    public RobotSettings getRobotSettingsFromStore(){
        return new RobotSettings(lsp);
    }

    public RobotSettings getRobotSettingsFromRobot(){
        return new RobotSettings(connection);
    }

    public List<String> getSerialPortNames(){
        return spp.getSerialPortNames();
    }

    public void connectToPort(String portName) throws PortConnectionException {
        connection = spp.connectToPort(portName);
    }

    public void exit(){
        if (connection!=null){
            connection.disconnect();
        }
        lsp.persist();
    }

}
