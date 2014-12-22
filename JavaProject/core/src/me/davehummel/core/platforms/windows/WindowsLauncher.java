package me.davehummel.core.platforms.windows;

import me.davehummel.core.Core;
import me.davehummel.core.providers.connection.rxtx.RXTXSerialPortProvider;

/**
 * Created by Dave on 12/20/2014.
 */
public class WindowsLauncher {

    public static void main(String[] parms){
        Core core = new Core();
        core.setLocalStoreProvider(new WindowsLocalStoreProvider());
        core.setSerialPortProvider(new RXTXSerialPortProvider());
        core.setUIProvider(new WindowsUIProvider());

        // Thread waits in start until lifecycle is closed
        core.start();
    }

}
