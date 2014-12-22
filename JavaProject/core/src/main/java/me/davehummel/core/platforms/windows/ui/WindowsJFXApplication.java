package me.davehummel.core.platforms.windows.ui;

import javafx.application.Application;
import javafx.stage.Stage;
import me.davehummel.core.platforms.windows.WindowsUIProvider;

/**
 * Created by Dave on 12/21/2014.
 */
public class WindowsJFXApplication extends Application {
    @Override
    public void start(Stage primaryStage) throws Exception {
        WindowsUIProvider.setup(primaryStage, this);
    }
}
