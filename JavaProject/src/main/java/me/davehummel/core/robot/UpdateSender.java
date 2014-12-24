package me.davehummel.core.robot;

import me.davehummel.core.providers.connection.PortConnectionException;

/**
 * Created by Dave on 12/21/2014.
 */
public class UpdateSender {
    final private RobotConnection connection;

    public UpdateSender(RobotConnection connection) {
        this.connection = connection;
    }

    public void sendUpdate(int speed, int turnAmount, boolean turnLeft) throws PortConnectionException {
        if (connection!=null){
            connection.sendLine(RobotConnection.UPDATE_COMMAND+speed+","+turnAmount+(turnLeft?"l":"r"));
            //connection.ask(RobotConnection.UPDATE_COMMAND,speed+","+turnAmount+(turnLeft?"l":"r"));
        }
    }

    public boolean switchExecuteMode() throws PortConnectionException {
        if (connection != null) {
          return  connection.ask(RobotConnection.EXECUTE_COMMAND, "").endsWith("true");
        }
        return false;
    }
}
