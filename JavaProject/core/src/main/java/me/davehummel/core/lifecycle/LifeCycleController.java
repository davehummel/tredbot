package me.davehummel.core.lifecycle;

import me.davehummel.core.IntegrationService;
import me.davehummel.core.SettingsScreenResponse;
import me.davehummel.core.providers.ui.ConnectionScreenResponse;
import me.davehummel.core.providers.ui.RCScreenResponse;

/**
 * Created by Dave on 12/20/2014.
 */
public class LifeCycleController {

    private final IntegrationService integration;

    public LifeCycleController(IntegrationService integration) {
        this.integration = integration;
    }

    public void start() {
        ConnectionScreenResponse connectionResponse = null;
        while (true) {
            if (connectionResponse == null) {
                connectionResponse = integration.showConnectionScreen();
                if (connectionResponse == null)
                    return;
                if (connectionResponse == ConnectionScreenResponse.REFRESH) {
                    connectionResponse = null;
                    continue;
                }
                if (connectionResponse.error != null) {
                    integration.showErrorScreen("Connection Failed", "Unable to connect to port:" + connectionResponse.portName, connectionResponse.error);
                    connectionResponse = null;
                    continue;
                }
            }

            // Show touch screen

            RCScreenResponse rcScreenResponse = integration.showRCScreen();

            if (rcScreenResponse == RCScreenResponse.LostConnection) {
                integration.showErrorScreen("Lost Connection", "Unable to communicate with robot.", null);
                integration.disconnect();
                connectionResponse = null;
                continue;
            }
            if (rcScreenResponse == RCScreenResponse.Settings) {
                SettingsScreenResponse response = integration.showSettingsScreen();
                if (response == SettingsScreenResponse.LostConnection) {
                    integration.showErrorScreen("Lost Connection", "Unable to send settings to robot.", null);
                    integration.disconnect();
                    connectionResponse = null;
                    continue;
                }
                continue;
            }
            if (rcScreenResponse == RCScreenResponse.Exit) {
                integration.exit();
                return;
            }
        }


    }

}
