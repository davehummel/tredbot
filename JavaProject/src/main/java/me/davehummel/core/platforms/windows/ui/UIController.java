package me.davehummel.core.platforms.windows.ui;

import me.davehummel.core.IntegrationService;
import me.davehummel.core.providers.ui.ScreenResponse;

/**
 * Created by Dave on 12/22/2014.
 */
public class UIController {

    private IntegrationService integrationService;

    public void setIntegrationService(IntegrationService integrationService){
        this.integrationService = integrationService;
    }

    protected void complete(ScreenResponse response){
        this.integrationService.nextStep(response);
    }
}
