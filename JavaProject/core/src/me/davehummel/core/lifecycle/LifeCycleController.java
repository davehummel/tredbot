package me.davehummel.core.lifecycle;

import me.davehummel.core.IntegrationService;
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

    public void start(){
        ConnectionScreenResponse connectionResponse = null;
        while (true) {
            if (connectionResponse == null) {
                connectionResponse = integration.showConnectionScreen();
                if (connectionResponse == null)
                    return;
                if (connectionResponse == ConnectionScreenResponse.REFRESH)
                    continue;
            }
            // Show touch screen

            RCScreenResponse rcScreenResponse = integration.showRCScreen();

            if (rcScreenResponse == RCScreenResponse.LostConnection){
                connectionResponse.connection.disconnect();
                connectionResponse = null;
                continue;
            }
            if (rcScreenResponse == RCScreenResponse.Settings){
               integration.showSettingsScreen();
               continue;
            }
            if (rcScreenResponse == RCScreenResponse.Exit){
                integration.exit();
                return;
            }
        }


    }

}
