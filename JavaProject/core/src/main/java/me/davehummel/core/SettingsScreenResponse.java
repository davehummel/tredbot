package me.davehummel.core;

import me.davehummel.core.robot.RobotSettings;

/**
 * Created by Dave on 12/20/2014.
 */
public class SettingsScreenResponse {

    public static final SettingsScreenResponse Done = new SettingsScreenResponse(false,false,null), LostConnection = new SettingsScreenResponse(false,false,null);

    public final boolean isUpload;
    public final boolean isSave;
    public final RobotSettings robotSettings;

    public SettingsScreenResponse(boolean isUpload, boolean isSave, RobotSettings robotSettings) {
        this.isUpload = isUpload;
        this.isSave = isSave;
        this.robotSettings = robotSettings;
    }
}
