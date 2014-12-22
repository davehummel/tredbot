package me.davehummel.core.platforms.windows;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Dialog;
import javafx.stage.Stage;
import me.davehummel.core.SettingsScreenResponse;
import me.davehummel.core.lifecycle.LifeCycleController;
import me.davehummel.core.platforms.windows.ui.ConnectionScreenController;
import me.davehummel.core.platforms.windows.ui.WindowsJFXApplication;
import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.ui.ConnectionScreenResponse;
import me.davehummel.core.providers.ui.RCScreenResponse;
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
    static private Stage stage;
    static volatile private LifeCycleController lifecycle;
    static volatile private Application app = null;

    public static void setup(Stage stage, Application app) {
        stage.setFullScreen(true);
        stage.setAlwaysOnTop(true);
        WindowsUIProvider.stage = stage;
        WindowsUIProvider.app = app;

        lifecycle.start();
    }

    public void start(LifeCycleController lifecycle) {

        this.lifecycle = lifecycle;

        Application.launch(WindowsJFXApplication.class);

    }


    @Override
    public ConnectionScreenResponse showConnectionScreen(String lastUsedPort, List<String> serialPortNames) {
        if (connectionScreen == null) {
            try {
                File file = new File("resources/ConnectionScreen.fxml");
                FXMLLoader fxmlLoader = new FXMLLoader(file.toURL());
                connectionScreenController = new ConnectionScreenController();
                connectionScreenController.setInitialValues(lastUsedPort,serialPortNames);

                fxmlLoader.setController(connectionScreenController);
                connectionScreen = (Parent) fxmlLoader.load();
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            }
        }

        stage.setTitle("Choose Serial Port");
        stage.setScene(new Scene(connectionScreen));
        stage.showAndWait();
        return connectionScreenController.getResponse();
    }

    @Override
    public void shutdown() {
        if (app!=null) {
            try {
                app.stop();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public SettingsScreenResponse showSettingsScreen(RobotSettings localSettings, RobotSettings robotSettings) {
        return null;
    }

    @Override
    public RCScreenResponse showRCScreen(StatusReceiver statusReceiver, UpdateSender updateSender) {
        return null;
    }

    @Override
    public void showErrorScreen(String title, String message, Exception error) {
        if (error!=null){
            Action response = Dialogs.create().title(title).message(message).showException(error);
        }else{
            Action response = Dialogs.create().title(title).message(message).showError();
        }
    }
}
