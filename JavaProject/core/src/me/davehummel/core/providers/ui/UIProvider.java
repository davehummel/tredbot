package me.davehummel.core.providers.ui;

import me.davehummel.core.SettingsScreenResponse;
import me.davehummel.core.lifecycle.LifeCycleController;
import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.robot.RobotSettings;
import me.davehummel.core.robot.StatusReceiver;
import me.davehummel.core.robot.UpdateSender;

import java.util.List;

/**
 * Created by davidhummel on 12/16/14.
 */
public interface UIProvider {

    void start(LifeCycleController lifeCycleController);

    ConnectionScreenResponse showConnectionScreen(String lastUsedPort, List<String> serialPortNames);

    void shutdown();

    SettingsScreenResponse showSettingsScreen(RobotSettings localSettings, RobotSettings robotSettings);

    RCScreenResponse showRCScreen(StatusReceiver statusReceiver, UpdateSender updateSender);

    void showErrorScreen(String message, PortConnectionException error);
}
