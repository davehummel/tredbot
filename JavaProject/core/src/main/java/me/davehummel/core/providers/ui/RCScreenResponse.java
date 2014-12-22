package me.davehummel.core.providers.ui;

import me.davehummel.core.robot.StatusReceiver;
import me.davehummel.core.robot.UpdateSender;

/**
 * Created by Dave on 12/20/2014.
 */
public class RCScreenResponse {
    public final static RCScreenResponse Settings = new RCScreenResponse(null,null), Exit = new RCScreenResponse(null,null), LostConnection = new RCScreenResponse(null,null);

    public final StatusReceiver statusReceiver;

    public final UpdateSender updateSender;


    public RCScreenResponse(StatusReceiver statusReceiver, UpdateSender updateSender) {
        this.statusReceiver = statusReceiver;
        this.updateSender = updateSender;
    }
}
