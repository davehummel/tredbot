package me.davehummel.core;

import me.davehummel.core.providers.connection.SerialPortProvider;
import me.davehummel.core.providers.localstore.LocalStoreProvider;
import me.davehummel.core.providers.ui.UIProvider;

/**
 * Created by davidhummel on 12/16/14.
 */
public class Core {

    SerialPortProvider spp;

    UIProvider uip;

    LocalStoreProvider lsp;

    public void setSerialPortProvider(SerialPortProvider serialPortProvider){
        this.spp = serialPortProvider;
    }

    public void setUIProvider(UIProvider uiProvider){
        this.uip = uiProvider;
    }

    public void start(){
        UIIntegration uiIntegration = new UIIntegration(spp,lsp);
    }

    public void setLocalStoreProvider(LocalStoreProvider localStoreProvider){
        this.lsp = localStoreProvider;
    }

}
