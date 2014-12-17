package me.davehummel.core;

import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.providers.localstore.LocalStoreProvider;
import me.davehummel.core.robot.RobotConnection;
import me.davehummel.core.robot.RobotSettings;

/**
 * Created by davidhummel on 12/16/14.
 */
public class UIIntegration {

    private SerialPortProvider spp;
    private LocalStoreProvider lsp;

    public void setRobotSettings(RobotSettings settings){
        lsp.setAll(settings.getMap());
    }

    public RobotSettings getRobotSettingsFromStore(){
        return null;
    }

    public RobotSettings getRobotSettingsFromRobot(){

        return null;
    }

    public String[] getSerialPortNames(){

        return null;
    }

    public RobotConnection connectPort(String portName) throws PortConnectionException {

        return null;
    }

    public void exit(){

    }

}
