package me.davehummel.core.platforms.windows.ui;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.Hyperlink;
import javafx.scene.control.ListView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.MouseEvent;
import me.davehummel.core.providers.ui.ConnectionScreenResponse;

import java.net.URL;
import java.util.List;
import java.util.ResourceBundle;

/**
 * Created by Dave on 12/21/2014.
 */
public class ConnectionScreenController extends UIController implements Initializable  {

    @FXML ListView<String> portList;

    @FXML Button connectButton;

    @FXML Hyperlink refreshLink;
    private List<String> portNames;
    private String lastUsedPort;
    private ConnectionScreenResponse response;


    @Override
    public void initialize(URL location, ResourceBundle resources) {
        portList.getItems().setAll(portNames);
        portList.getSelectionModel().select(lastUsedPort);
        EventHandler<? super MouseEvent> dblClickHandler = new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent mouseEvent) {
                if (mouseEvent.getClickCount() == 2)
                connectButton.fire();
            }
        };
        portList.setOnMouseClicked( dblClickHandler );
        portList.setOnKeyTyped( keyEvent -> {
            if (keyEvent.getCode() == KeyCode.ENTER){
                connectButton.fire();
            }
        });

        EventHandler<ActionEvent> btnHandler = actionEvent -> {
            response =  new ConnectionScreenResponse(portList.getSelectionModel().getSelectedItem());
            complete(response);
        };

        connectButton.setOnAction( btnHandler );

        connectButton.getParent().setOnKeyTyped( keyEvent -> {
            if (keyEvent.getCode() == KeyCode.ENTER) {
                response =  new ConnectionScreenResponse(portList.getSelectionModel().getSelectedItem());
                complete(response);
            }else{
                response =  new ConnectionScreenResponse(portList.getSelectionModel().getSelectedItem(),keyEvent.getCharacter());
                complete(response);
            }
        });

        refreshLink.setOnAction(event -> {
            complete(ConnectionScreenResponse.REFRESH);
        });

    }


    public void setInitialValues(String lastUsedPort, List<String> serialPortNames) {
        this.lastUsedPort = lastUsedPort;
        this.portNames = serialPortNames;
    }

}
