package me.davehummel.core.platforms.windows;

import javafx.application.Application;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import me.davehummel.core.IntegrationService;
import me.davehummel.core.lifecycle.LifeCycleController;
import me.davehummel.core.platforms.windows.ui.ConnectionScreenController;
import me.davehummel.core.platforms.windows.ui.RCScreenController;
import me.davehummel.core.platforms.windows.ui.SettingsScreenController;
import me.davehummel.core.platforms.windows.ui.WindowsJFXApplication;
import me.davehummel.core.providers.ui.UIProvider;
import me.davehummel.core.robot.RobotSettings;
import me.davehummel.core.robot.StatusReceiver;
import me.davehummel.core.robot.UpdateSender;
import org.controlsfx.control.action.Action;
import org.controlsfx.dialog.Dialogs;

import java.io.File;
import java.io.IOException;
import java.util.List;

/**
 * Created by Dave on 12/21/2014.
 */
public class WindowsUIProvider implements UIProvider {

    private Parent connectionScreen = null;
    private ConnectionScreenController connectionScreenController;
    private Parent rcScreen = null;
    private RCScreenController rcScreenController;
    private Parent settingsScreen = null;
    private SettingsScreenController settingsScreenController;

    static private Stage stage;
    static volatile private LifeCycleController lifecycle;
    static volatile private Application app = null;

    public static void setup(Stage stage, Application app) {
//        stage.setFullScreen(true);
//        stage.setAlwaysOnTop(true);
        WindowsUIProvider.stage = stage;
        WindowsUIProvider.app = app;

        lifecycle.nextStep(null);
    }

    public void start(LifeCycleController lifecycle) {

        this.lifecycle = lifecycle;

        Application.launch(WindowsJFXApplication.class);

    }


    @Override
    public void showConnectionScreen(String lastUsedPort, List<String> serialPortNames, final IntegrationService integrationService) {
        try {
            File file = new File("resources/ConnectionScreen.fxml");
            FXMLLoader fxmlLoader = new FXMLLoader(file.toURL());
            connectionScreenController = new ConnectionScreenController();
            connectionScreenController.setIntegrationService(integrationService);
            fxmlLoader.setController(connectionScreenController);
            connectionScreenController.setInitialValues(lastUsedPort, serialPortNames);
            connectionScreen = fxmlLoader.load();
        } catch (IOException e) {
            e.printStackTrace();
            integrationService.exit();
            return;
        }


        updateStage("Choose Serial Port", connectionScreen, () -> integrationService.exit());


    }

    @Override
    public void shutdown() {
        if (app != null) {
            try {
                if (stage!=null)
                    stage.close();
                app.stop();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void showSettingsScreen(RobotSettings localSettings, RobotSettings robotSettings, IntegrationService integrationService) {

        try {
            File file = new File("resources/SettingsScreen.fxml");
            FXMLLoader fxmlLoader = new FXMLLoader(file.toURL());
            settingsScreenController = new SettingsScreenController();
            settingsScreenController.setIntegrationService(integrationService);
            fxmlLoader.setController(settingsScreenController);
            settingsScreenController.setInitialValues(localSettings, robotSettings);
            settingsScreen = fxmlLoader.load();
        } catch (IOException e) {
            e.printStackTrace();
            integrationService.exit();
            return;
        }


        updateStage("Setup Robot", settingsScreen, () -> integrationService.showRCScreen());
    }

    @Override
    public void showRCScreen(StatusReceiver statusReceiver, UpdateSender updateSender, IntegrationService integrationService) {

        try {
            File file = new File("resources/RCScreen.fxml");
            FXMLLoader fxmlLoader = new FXMLLoader(file.toURL());
            rcScreenController = new RCScreenController();
            rcScreenController.setIntegrationService(integrationService);
            fxmlLoader.setController(rcScreenController);
            rcScreenController.setInitialValues(integrationService.getUpdateSender(),integrationService.getStatusReceiver());
            rcScreen = fxmlLoader.load();
        } catch (IOException e) {
            e.printStackTrace();
            integrationService.exit();
        }


        updateStage("Drive!", rcScreen, () -> integrationService.exit());
    }

    @Override
    public void showErrorScreen(String title, String message, Exception error) {
        if (error != null) {
            Action response = Dialogs.create().title(title).message(message).showException(error);
        } else {
            Action response = Dialogs.create().title(title).message(message).showError();
        }
    }

    private void updateStage(String title, Parent root, Runnable onExit) {
        stage.setTitle(title);
        Scene scene = stage.getScene();
        if (scene == null)
            scene = new Scene(root);
        else
            scene.setRoot(root);
        stage.setScene(scene);
        stage.setOnHidden(new EventHandler<WindowEvent>() {
            @Override
            public void handle(WindowEvent event) {
                onExit.run();
            }
        });

        if (!stage.isShowing())
            stage.show();
    }
}
