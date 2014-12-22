package me.davehummel.core;

import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.providers.localstore.LocalStoreProvider;
import me.davehummel.core.providers.ui.UIProvider;
import me.davehummel.core.lifecycle.LifeCycleController;

/**
 * Created by davidhummel on 12/16/14.
 */
public class Core {

    SerialPortProvider spp;

    UIProvider uip;

    LocalStoreProvider lsp;

    private CloseListener closeListener = new CloseListener() {
        public void onClose() {

        }
    };

    public void setSerialPortProvider(SerialPortProvider serialPortProvider){
        this.spp = serialPortProvider;
    }

    public void setUIProvider(UIProvider uiProvider){
        this.uip = uiProvider;
    }

    public void setLocalStoreProvider(LocalStoreProvider localStoreProvider){
        this.lsp = localStoreProvider;
    }

    public void start(){
        IntegrationService integration = new IntegrationService(spp,lsp,uip);
        integration.setCloseListener(closeListener);
        LifeCycleController lifeCycle = new LifeCycleController(integration);
        uip.start(lifeCycle);
        uip.shutdown();
        spp.shutdown();
        lsp.shutdown();
    }



}
