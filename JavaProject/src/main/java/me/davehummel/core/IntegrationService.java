package me.davehummel.core;

import me.davehummel.core.lifecycle.LifeCycleController;
import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.providers.localstore.LocalStoreProvider;
import me.davehummel.core.providers.ui.ConnectionScreenResponse;
import me.davehummel.core.providers.ui.ScreenResponse;
import me.davehummel.core.providers.ui.SettingsScreenResponse;
import me.davehummel.core.providers.ui.UIProvider;
import me.davehummel.core.robot.RobotConnection;
import me.davehummel.core.robot.RobotSettings;
import me.davehummel.core.robot.StatusReceiver;
import me.davehummel.core.robot.UpdateSender;

/**
 * Created by davidhummel on 12/16/14.
 */
public class IntegrationService {

    public static final String LAST_USED_PORT = "lastPort";
    private  LifeCycleController lifeCycle;
    private final SerialPortProvider spp;
    private final LocalStoreProvider lsp;
    private final UIProvider uip;
    private RobotConnection connection;
    private CloseListener closeListener;

    public IntegrationService(SerialPortProvider spp, LocalStoreProvider lsp, UIProvider uip) {
        this.spp = spp;
        this.lsp = lsp;
        this.uip = uip;

    }


//    public boolean switchDebugMode() throws IOException {
//        if (connection != null) {
//            return connection.ask(RobotConnection.DEBUG_COMMAND, "").endsWith("true");
//        }
//        return false;
//    }
//


    public void exit() {
        if (connection != null) {
            connection.disconnect();
        }
        lsp.persist();
        uip.shutdown();
        closeListener.onClose();
    }

    public void setCloseListener(CloseListener closeListener) {
        this.closeListener = closeListener;
    }

    public void showConnectionScreen() {
        String lastUsedPort = lsp.get(LAST_USED_PORT);
        uip.showConnectionScreen(lastUsedPort, spp.getSerialPortNames(),this);

    }

    public void showRCScreen() {
        uip.showRCScreen(connection.getStatusReceiver(), connection.getUpdateSender(), this);
    }


    public void showGyroScreen() {
        uip.showGyroScreen(connection.getLineReceiver(),this);
    }

    public void showSettingsScreen() {
        RobotSettings localSettings = new RobotSettings(lsp);
        try {
            RobotSettings robotSettings = new RobotSettings(connection);
            uip.showSettingsScreen(localSettings,robotSettings,this);
        } catch (PortConnectionException e) {
            showErrorScreen("Unable to connect", "Failed to get settings from robot",e);
            showConnectionScreen();
        } catch (Exception e){
            showErrorScreen("Unknown Failure", e.getMessage(), e);
            exit();
        }
    }

    public void showErrorScreen(String title, String message, Exception error) {
        if (error != null)
            error.printStackTrace();
        uip.showErrorScreen(title,message,error);
    }

    public void setLifeCycleController(LifeCycleController lifeCycleController){
        this.lifeCycle = lifeCycleController;
    }

    public void nextStep(ScreenResponse response) {
        if (response instanceof ConnectionScreenResponse){
            ConnectionScreenResponse csr = (ConnectionScreenResponse) response;

            if(csr.portName!=null){
                try {
                    lsp.set(LAST_USED_PORT,csr.portName);
                    connection = spp.connectToPort(csr.portName);
                } catch (PortConnectionException e) {
                    response = new ConnectionScreenResponse(csr.portName,e);
                }
            }
        } else if (response instanceof SettingsScreenResponse){
            SettingsScreenResponse ssr = (SettingsScreenResponse) response;
            if (ssr.isSave){
                ssr.robotSettings.writeToStore(lsp);
            }
            if (ssr.isUpload){
                try {
                    ssr.robotSettings.writeToRobot(connection);
                } catch (PortConnectionException e) {
                    showErrorScreen("Connection lost",e.getMessage(),e);
                    lifeCycle.nextStep(ConnectionScreenResponse.REFRESH);
                    return;
                }
            }
        }
        lifeCycle.nextStep(response);
    }

    public UpdateSender getUpdateSender(){
        return connection.getUpdateSender();
    }

    public StatusReceiver getStatusReceiver(){
        return connection.getStatusReceiver();
    }

    public void pushSettings() throws PortConnectionException {
        RobotSettings settings = new RobotSettings(lsp);
        if (settings.isCustom()){
            settings.writeToRobot(connection);
        }
    }

}
