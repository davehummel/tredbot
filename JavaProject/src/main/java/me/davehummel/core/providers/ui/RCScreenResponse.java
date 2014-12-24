package me.davehummel.core.providers.ui;

import me.davehummel.core.robot.StatusReceiver;
import me.davehummel.core.robot.UpdateSender;

/**
 * Created by Dave on 12/20/2014.
 */
public class RCScreenResponse implements ScreenResponse{
    public final static RCScreenResponse Settings = new RCScreenResponse(-1), Exit = new RCScreenResponse(-1), LostConnection = new RCScreenResponse(-1);

    public final Integer num;

    public RCScreenResponse(Integer buttonNumber) {
       this.num = buttonNumber;
    }

}
