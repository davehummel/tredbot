package me.davehummel.core.providers.ui;

import me.davehummel.core.robot.RobotConnection;

/**
 * Created by Dave on 12/20/2014.
 */
public class ConnectionScreenResponse {

    public static final ConnectionScreenResponse REFRESH = new ConnectionScreenResponse(null,null);

    public final String portName;
    public final RobotConnection connection;


    public ConnectionScreenResponse(String portName, RobotConnection connection) {
        this.portName = portName;
        this.connection = connection;
    }

}
