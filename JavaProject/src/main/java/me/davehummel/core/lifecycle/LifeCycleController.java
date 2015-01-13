package me.davehummel.core.lifecycle;

import me.davehummel.core.IntegrationService;
import me.davehummel.core.providers.ui.ConnectionScreenResponse;
import me.davehummel.core.providers.ui.RCScreenResponse;
import me.davehummel.core.providers.ui.ScreenResponse;
import me.davehummel.core.providers.ui.SettingsScreenResponse;

/**
 * Created by Dave on 12/20/2014.
 */
public class LifeCycleController {

    private final IntegrationService integration;

    public LifeCycleController(IntegrationService integration) {
        this.integration = integration;
    }

    public void nextStep(ScreenResponse response) {
        if (response == null) {
            integration.showConnectionScreen();
        }
        if (response instanceof ConnectionScreenResponse) {
            ConnectionScreenResponse csr = (ConnectionScreenResponse) response;
            if (csr == ConnectionScreenResponse.EXIT) {
                integration.exit();
                return;
            }
            if (csr == ConnectionScreenResponse.REFRESH) {
                integration.showConnectionScreen();
                return;
            }
            if (csr.error != null) {
                integration.showErrorScreen("Connection Failed", "Unable to connect to port:" + csr.portName, csr.error);
                return;
            }
            if (csr.extra!=null) {
                if (csr.extra.equalsIgnoreCase("q")) {
                    integration.showGyroScreen();
                    return;
                }
            }
//            try {
//                integration.pushSettings();
//            } catch (Exception e){
//                integration.showErrorScreen("Connection Failed", "Unable to connect to port:" + csr.portName, csr.error);
//                integration.showConnectionScreen();
//                return;
//            }
            integration.showRCScreen();
            return;
        }
        if (response instanceof RCScreenResponse) {
            RCScreenResponse rcResponse = (RCScreenResponse) response;
            if (rcResponse == RCScreenResponse.Exit) {
                integration.exit();
                return;
            }
            if (rcResponse == RCScreenResponse.LostConnection) {
                integration.showConnectionScreen();
                return;
            }
            if (rcResponse == RCScreenResponse.Settings) {
                integration.showSettingsScreen();
                return;
            }
        }
        if (response instanceof SettingsScreenResponse) {
            integration.showRCScreen();
            return;
        }

    }

//    public void start() {
//        ConnectionScreenResponse connectionResponse = null;
//        while (true) {
//            if (connectionResponse == null) {
//                connectionResponse = integration.showConnectionScreen();
//                if (connectionResponse == null)
//                    return;
//                if (connectionResponse == ConnectionScreenResponse.REFRESH) {
//                    connectionResponse = null;
//                    continue;
//                }
//                if (connectionResponse.error != null) {
//                    integration.showErrorScreen("Connection Failed", "Unable to connect to port:" + connectionResponse.portName, connectionResponse.error);
//                    connectionResponse = null;
//                    continue;
//                }
//            }
//
//            // Show touch screen
//
//            RCScreenResponse rcScreenResponse = integration.showRCScreen();
//
//            if (rcScreenResponse == RCScreenResponse.LostConnection) {
//                integration.showErrorScreen("Lost Connection", "Unable to communicate with robot.", null);
//                integration.disconnect();
//                connectionResponse = null;
//                continue;
//            }
//            if (rcScreenResponse == RCScreenResponse.Settings) {
//                SettingsScreenResponse response = integration.showSettingsScreen();
//                if (response == SettingsScreenResponse.LostConnection) {
//                    integration.showErrorScreen("Lost Connection", "Unable to send settings to robot.", null);
//                    integration.disconnect();
//                    connectionResponse = null;
//                    continue;
//                }
//                continue;
//            }
//            if (rcScreenResponse == RCScreenResponse.Exit) {
//                integration.exit();
//                return;
//            }
//        }
//
//
//    }

}
