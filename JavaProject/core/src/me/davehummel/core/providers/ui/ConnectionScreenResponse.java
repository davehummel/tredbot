package me.davehummel.core.providers.ui;

import me.davehummel.core.robot.RobotConnection;

/**
 * Created by Dave on 12/20/2014.
 */
public class ConnectionScreenResponse {

    public static final ConnectionScreenResponse REFRESH = new ConnectionScreenResponse(null);

    public final String portName;

    public ConnectionScreenResponse(String portName) {
        this.portName = portName;
    }

}
