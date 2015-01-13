package me.davehummel.core.providers.ui;

import me.davehummel.core.IntegrationService;
import me.davehummel.core.lifecycle.LifeCycleController;
import me.davehummel.core.robot.LineReceiver;
import me.davehummel.core.robot.RobotSettings;
import me.davehummel.core.robot.StatusReceiver;
import me.davehummel.core.robot.UpdateSender;

import java.util.List;

/**
 * Created by davidhummel on 12/16/14.
 */
public interface UIProvider {

    void start(LifeCycleController lifeCycleController);

    void showConnectionScreen(String lastUsedPort, List<String> serialPortNames, IntegrationService integrationService);

    void shutdown();

    void showSettingsScreen(RobotSettings localSettings, RobotSettings robotSettings, IntegrationService integrationService);

    void showRCScreen(StatusReceiver statusReceiver, UpdateSender updateSender, IntegrationService integrationService);

    void showErrorScreen(String title, String message, Exception error);

    void showGyroScreen(LineReceiver lineReceiver, IntegrationService integrationService);
}
