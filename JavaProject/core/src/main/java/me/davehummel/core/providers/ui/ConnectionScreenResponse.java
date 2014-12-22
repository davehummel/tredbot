package me.davehummel.core.providers.ui;

import me.davehummel.core.platforms.windows.ui.ConnectionScreenController;
import me.davehummel.core.robot.RobotConnection;

/**
 * Created by Dave on 12/20/2014.
 */
public class ConnectionScreenResponse {

    public static final ConnectionScreenResponse REFRESH = new ConnectionScreenResponse(null);

    public final String portName;

    public final Exception error;

    public ConnectionScreenResponse(String portName) {
        this.portName = portName;
        this.error = null;
    }

    public ConnectionScreenResponse(String portName, Exception e){
        this.portName = portName;
        this.error = e;
    }

}
