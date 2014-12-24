package me.davehummel.core.providers.ui;

import me.davehummel.core.robot.RobotSettings;

/**
 * Created by Dave on 12/20/2014.
 */
public class SettingsScreenResponse implements ScreenResponse{

    public final boolean isUpload;
    public final boolean isSave;
    public final RobotSettings robotSettings;

    public SettingsScreenResponse(boolean isUpload, boolean isSave, RobotSettings robotSettings) {
        this.isUpload = isUpload;
        this.isSave = isSave;
        this.robotSettings = robotSettings;
    }
}
