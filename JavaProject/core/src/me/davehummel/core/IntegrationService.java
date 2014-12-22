package me.davehummel.core;

import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.providers.localstore.LocalStoreProvider;
import me.davehummel.core.providers.ui.ConnectionScreenResponse;
import me.davehummel.core.providers.ui.RCScreenResponse;
import me.davehummel.core.providers.ui.UIProvider;
import me.davehummel.core.robot.RobotConnection;
import me.davehummel.core.robot.RobotSettings;
import me.davehummel.core.robot.StatusReceiver;
import me.davehummel.core.robot.UpdateSender;

import java.io.IOException;
import java.util.List;

/**
 * Created by davidhummel on 12/16/14.
 */
public class IntegrationService {

    public static final String LAST_USED_PORT = "lastPort";

    private SerialPortProvider spp;
    private LocalStoreProvider lsp;
    private UIProvider uip;
    private RobotConnection connection;
    private CloseListener closeListener;

    public IntegrationService(SerialPortProvider spp, LocalStoreProvider lsp, UIProvider uip) {
        this.spp = spp;
        this.lsp = lsp;
        this.uip = uip;
    }

    public void storeRobotSettings(RobotSettings robotSettings) {
        robotSettings.writeToStore(lsp);
    }

    public RobotSettings getRobotSettingsFromStore() {
        return new RobotSettings(lsp);
    }

    public RobotSettings getRobotSettingsFromRobot() throws IOException {
        return new RobotSettings(connection);
    }

    public List<String> getSerialPortNames() {
        return spp.getSerialPortNames();
    }

    public void connectToPort(String portName) throws PortConnectionException {
        connection = spp.connectToPort(portName);
    }

    public void updateSpeed(int speed, int turn, boolean turnLeft) throws IOException {
        if (connection != null) {
            connection.ask(RobotConnection.UPDATE_COMMAND, speed + "," + turn + (turnLeft ? "l" : "r"));
        }
    }

    public boolean switchDebugMode() throws IOException {
        if (connection != null) {
            return connection.ask(RobotConnection.DEBUG_COMMAND, "").endsWith("true");
        }
        return false;
    }

    public boolean switchExecuteMode() throws IOException {
        if (connection != null) {
            connection.ask(RobotConnection.EXECUTE_COMMAND, "").endsWith("true");
        }
        return false;
    }

    public void exit() {
        if (connection != null) {
            connection.disconnect();
        }
        lsp.persist();
        closeListener.onClose();
    }

    public void setCloseListener(CloseListener closeListener) {
        this.closeListener = closeListener;
    }

    public ConnectionScreenResponse showConnectionScreen() {
        String lastUsedPort = lsp.get(LAST_USED_PORT);
        ConnectionScreenResponse response = uip.showConnectionScreen(lastUsedPort, spp.getSerialPortNames());
        if (response != null)
            lsp.set(LAST_USED_PORT, response.portName);
        return response;
    }

    public RCScreenResponse showRCScreen() {
        ;

        RCScreenResponse response = uip.showRCScreen(connection.getStatusReceiver(), connection.getUpdateSender());
        return response;
    }

    public SettingsScreenResponse showSettingsScreen() {
        RobotSettings localSettings = new RobotSettings(lsp);
        try {
            RobotSettings robotSettings = new RobotSettings(connection);
            SettingsScreenResponse response = uip.showSettingsScreen(localSettings, robotSettings);
            if (response.isUpload) {
                response.robotSettings.writeToRobot(connection);
            }
            if (response.isSave) {
                response.robotSettings.writeToStore(lsp);
            }
            return SettingsScreenResponse.Done;
        } catch (IOException e) {
            return SettingsScreenResponse.LostConnection;
        }
    }

    public void disconnect() {
        if (connection != null)
            connection.disconnect();
    }

    public void showErrorScreen(String message, PortConnectionException error) {
        uip.showErrorScreen(message,error);
    }
}
