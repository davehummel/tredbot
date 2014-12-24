package me.davehummel.core.platforms.windows.ui;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TextField;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.paint.Color;
import me.davehummel.core.providers.ui.SettingsScreenResponse;
import me.davehummel.core.robot.RobotSettings;

import java.net.URL;
import java.util.ResourceBundle;

/**
 * Created by Dave on 12/21/2014.
 */
public class SettingsScreenController extends UIController implements Initializable  {

    private static final Background ERROR_BACK = new Background(new BackgroundFill(Color.ROSYBROWN,null,null));
    private static final Background OK_BACK = new Background(new BackgroundFill(Color.MINTCREAM,null,null));
    @FXML
    private TextField stepInterval;

    @FXML
    private TextField maxSpeedDelta;

    @FXML
    private TextField stoppedTurnInterval;

    @FXML
    private TextField fastTurnInterval;

    @FXML
    private TextField stoppedTurnSpeed;

    @FXML
    private TextField fastTurnSpeed;

    @FXML
    private Button saveButton;

    @FXML
    private CheckBox rememberCheck;
    private RobotSettings fileSettings;
    private RobotSettings remoteSettings;

    @Override
    public void initialize(URL location, ResourceBundle resources) {
            saveButton.setOnAction(event -> {
                try {
                    RobotSettings robotSettings = new RobotSettings(getInt(stepInterval),getInt(maxSpeedDelta),getInt(stoppedTurnInterval),getInt(fastTurnInterval),getInt(stoppedTurnSpeed),getInt(fastTurnSpeed));
                    complete(new SettingsScreenResponse(rememberCheck.isSelected(),true,robotSettings));
                } catch (NullPointerException e){
                    return;
                }
            });

        RobotSettings active = fileSettings;
        if (fileSettings.getStepInterval() == -1){
            active = remoteSettings;
        }

        stepInterval.setText(Integer.toString(active.getStepInterval()));
        maxSpeedDelta.setText(Integer.toString(active.getSpeedDeltaPerInterval()));
        stoppedTurnInterval.setText(Integer.toString(active.getStoppedTurnTime()));
        fastTurnInterval.setText(Integer.toString(active.getFastTurnTime()));
        stoppedTurnSpeed.setText(Integer.toString(active.getStoppedTurnSpeed()));
        fastTurnSpeed.setText(Integer.toString(active.getFastTurnSpeed()));
    }

    public void setInitialValues(RobotSettings fileSettings,RobotSettings remoteSettings){
        this.fileSettings = fileSettings;
        this.remoteSettings = remoteSettings;
    }

    private int getInt(TextField field) {
        try{
            int i =  Integer.parseInt(field.getText());
            field.setBackground(null);
            return i;
        } catch (Exception e){
            setError(field);
            throw new NullPointerException();
        }
    }

    private void setError(TextField field) {
        field.setBackground(ERROR_BACK);
    }

}
