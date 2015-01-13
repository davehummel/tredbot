package me.davehummel.core.platforms.windows.ui;

import javafx.application.Platform;
import javafx.beans.InvalidationListener;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Shape;
import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.ui.RCScreenResponse;
import me.davehummel.core.robot.StatusReceiver;
import me.davehummel.core.robot.UpdateSender;

import java.net.URL;
import java.util.ResourceBundle;

/**
 * Created by Dave on 12/21/2014.
 */
public class RCScreenController extends UIController implements Initializable {

    private static final int MINSPEED = 25;
    private static final int MAX_UNSIGNED_INT = 65535;
    private static final int MINTURN = 2000;
    private Paint disabledFill, enabledFill = null;


    @FXML
    private AnchorPane background;

    @FXML
    private ImageView circleImage;

    @FXML
    private Group button1;
    @FXML
    private Group button2;
    @FXML
    private Group button3;
    @FXML
    private Group button4;
    @FXML
    private Group button5;
    @FXML
    private Group button6;
    @FXML
    private Group button7;
    @FXML
    private Group button8;
    @FXML
    private Group button9;

    @FXML
    private Group settingsButton;

    @FXML
    private Group exitButton;

    @FXML
    private Circle touchPoint;

    @FXML
    private Circle robotPoint;

    @FXML
    private Circle centerPoint;

    volatile private int currentAngle = 0;

    volatile private int currentSpeed = 0;


    private int targetAngle = 0;

    private int targetSpeed = 0;

    private UpdateSender updateSender;
    private StatusReceiver statusReceiver;


    @Override
    public void initialize(URL location, ResourceBundle resources) {
        enabledFill = ((Shape) button1.getChildren().get(0)).getFill();
        disabledFill = Color.FIREBRICK;

        statusReceiver.setListener((speed, angle) -> {
            currentSpeed = speed;
            currentAngle = angle;

            Platform.runLater(() -> {

                touchPoint.setVisible(currentAngle != targetAngle);
                updatePoint(robotPoint, currentSpeed, currentAngle);
            });

        });

        robotPoint.setVisible(true);
        touchPoint.setVisible(false);
        centerPoint.setVisible(true);

        touchPoint.setMouseTransparent(true);
        robotPoint.setMouseTransparent(true);
        centerPoint.setMouseTransparent(true);


        button1.setOnMouseClicked(event -> {
            try {
                boolean enabled = updateSender.switchExecuteMode();
                ((Shape) button1.getChildren().get(0)).setFill(enabled ? enabledFill : disabledFill);
            } catch (PortConnectionException e) {
                complete(RCScreenResponse.LostConnection);
            }
        });

        button2.setOnMouseClicked(event -> {
            try {
                boolean enabled = updateSender.switchDebugMode();
                ((Shape) button2.getChildren().get(0)).setFill(enabled ? enabledFill : disabledFill);
            } catch (PortConnectionException e) {
                complete(RCScreenResponse.LostConnection);
            }
        });

        button3.setOnMouseClicked(event -> {
            try {
                updateSender.zeroCommand();
            } catch (PortConnectionException e) {
                complete(RCScreenResponse.LostConnection);
            }
        });

        button4.setOnMouseClicked(event -> {
            try {
                updateSender.sendUpdate(-100,targetAngle);
            } catch (PortConnectionException e) {
                complete(RCScreenResponse.LostConnection);
            }
        });

        ((Shape) button1.getChildren().get(0)).setFill(disabledFill);
        ((Shape) button2.getChildren().get(0)).setFill(disabledFill);

        exitButton.setOnMouseClicked(event -> {
            complete(RCScreenResponse.Exit);
        });

        settingsButton.setOnMouseClicked(event -> {
            complete(RCScreenResponse.Settings);
        });

        Image image = new Image("file:resources/chalky_black.png");
        BackgroundImage backgroundImage = new BackgroundImage(image, BackgroundRepeat.REPEAT, BackgroundRepeat.REPEAT, BackgroundPosition.CENTER, BackgroundSize.DEFAULT);
        background.setBackground(new Background(backgroundImage));

        image = new Image("file:resources/circle.png");
        circleImage.setImage(image);
        circleImage.setPreserveRatio(true);
        circleImage.setSmooth(true);


        circleImage.setOnTouchPressed(value -> {
            int speed = 0, angle = 0;

            double x, y;
            x = value.getTouchPoint().getX() / circleImage.getFitWidth();
            y = value.getTouchPoint().getY() / circleImage.getFitHeight();

            if (x <= 0)
                x = 0.0001;
            if (x > 1)
                x = 1;
            if (y <= 0)
                y = 0.0001;
            if (y > 1)
                y = 1;

            x = x * 2 - 1;
            y = y * 2 - 1;

            double distance = Math.sqrt(x * x + y * y);

            if (distance > 1) {
                return;
            }

            speed = (int) (distance * 255);

            if (speed < MINSPEED)
                speed = 0;

            double rads = Math.atan2(y, x);
            rads = rads+ Math.PI/2;
            while (rads <0 || rads > 2*Math.PI){
                if (rads < 0)
                    rads+= 2*Math.PI;
                else
                    rads-= 2*Math.PI;
            }
            angle = (int) (rads/(2*Math.PI) * MAX_UNSIGNED_INT);

            if (Math.abs(Math.abs(angle)-Math.abs(currentAngle)) < MINTURN)
                angle = currentAngle;

            if (speed > 0) {
                touchPoint.setVisible(true);
                targetAngle = angle;
                targetSpeed = speed;
            } else {
                touchPoint.setVisible(false);
                targetSpeed = 0;
            }

            updatePoint(touchPoint, targetSpeed, targetAngle);
            try {
                updateSender.sendUpdate(targetSpeed, targetAngle);
            } catch (PortConnectionException e) {
                complete(RCScreenResponse.LostConnection);
            }
        });

 //       circleImage.setOnTouchMoved(circleImage.getOnTouchPressed());

        circleImage.layoutXProperty().addListener(observable -> {
            double x = .5 * circleImage.getFitWidth();
            centerPoint.setCenterX(x + circleImage.getLayoutX());
        });

        circleImage.layoutYProperty().addListener(observable -> {
            double y = .5 * circleImage.getFitHeight();
            centerPoint.setCenterY(y + circleImage.getLayoutY());
        });


    }

    public void attacheResizeListeners(){
        InvalidationListener listener = observable-> {
            double height = background.getHeight() - 38;
            double width = background.getWidth() - 38;

            width = width - 180;
            if (height > width) {
                circleImage.setFitHeight(width);
                circleImage.setFitWidth(width);
            } else {
                circleImage.setFitHeight(height);
                circleImage.setFitWidth(height);
            }

            double x = .5 * circleImage.getFitWidth();
            double y = .5 * circleImage.getFitHeight();

            x = x + circleImage.getLayoutX();
            y = y + circleImage.getLayoutY();

            centerPoint.setRadius(circleImage.getFitWidth() / 17.0);
            centerPoint.setCenterY(y);
            centerPoint.setCenterX(x);

            touchPoint.setRadius(circleImage.getFitWidth() / 14.0);
            robotPoint.setRadius(circleImage.getFitWidth() / 16.0);
        };
        background.getScene().widthProperty().addListener(listener);
        background.getScene().heightProperty().addListener(listener);
      ;
        Platform.runLater(()-> {
            listener.invalidated(null);
        });

    }

    private void updatePoint(Circle touchPoint, int currentSpeed, int currentAngle) {
        double x, y;
        double angle  = ((double)currentAngle / (double)MAX_UNSIGNED_INT) * 2*Math.PI;
      // TODO shift Angle
        angle = angle - Math.PI / 2;
        double speed = currentSpeed;
        if (speed<MINSPEED)
            speed = MINSPEED;
        speed = speed / 255.0;
        if (speed < .05)
            speed = .1;

        x = Math.cos(angle) * speed;
        y = Math.sin(angle) * speed;

        x = (x + 1) / 2;
        y = (y + 1) / 2;

        x = x * circleImage.getFitWidth();
        y = y * circleImage.getFitHeight();

        x = x + circleImage.getLayoutX();
        y = y + circleImage.getLayoutY();

        touchPoint.setCenterX(x);
        touchPoint.setCenterY(y);

    }

    public void setInitialValues(UpdateSender updateSender, StatusReceiver statusReceiver) {
        this.updateSender = updateSender;
        this.statusReceiver = statusReceiver;
    }
}
