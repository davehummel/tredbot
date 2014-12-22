package me.davehummel.core.robot;

import java.io.IOException;

/**
 * Created by Dave on 12/21/2014.
 */
public class UpdateSender {
    final private RobotConnection connection;

    public UpdateSender(RobotConnection connection) {
        this.connection = connection;
    }

    public void sendUpdate(int speed, int turnAmount, boolean turnLeft) throws IOException {
        if (connection!=null){
            connection.ask(RobotConnection.UPDATE_COMMAND,speed+","+turnAmount+(turnLeft?"l":"r"));
        }
    }
}
