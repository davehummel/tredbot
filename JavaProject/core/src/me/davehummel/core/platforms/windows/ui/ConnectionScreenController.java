package me.davehummel.core.platforms.windows.ui;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.Hyperlink;
import javafx.scene.control.ListView;
import javafx.scene.layout.AnchorPane;

import java.net.URL;
import java.util.List;
import java.util.ResourceBundle;

/**
 * Created by Dave on 12/21/2014.
 */
public class ConnectionScreenController implements Initializable {

    @FXML ListView<String> portList;

    @FXML Button connectButton;

    @FXML Hyperlink refreshLink;
    private List<String> portNames;
    private String lastUsedPort;


    @Override
    public void initialize(URL location, ResourceBundle resources) {
        portList.getItems().setAll(portNames);
        portList.getSelectionModel().select(lastUsedPort);
    }

    public void setInitialValues(String lastUsedPort, List<String> serialPortNames) {
        this.lastUsedPort = lastUsedPort;
        this.portNames = serialPortNames;
    }
}
