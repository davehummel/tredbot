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

    private static final int MINSPEED = 30;
    private static final int MAX_UNSIGNED_INT = 65535;
    private static final int MINTURN = 3000;
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

    volatile private double referenceTurn = 0;

    volatile private double referenceSpeed = 0;

    volatile private boolean referenceTL = false;

    volatile private double refrenceAngle = 0;


    private double targetTurn = 0;

    private double targetSpeed = 0;

    private boolean targetTL = false;

    private UpdateSender updateSender;
    private StatusReceiver statusReceiver;


    @Override
    public void initialize(URL location, ResourceBundle resources) {
        enabledFill = ((Shape) button1.getChildren().get(0)).getFill();
        disabledFill = Color.FIREBRICK;

        statusReceiver.setListener((speed, turnRemaining, turnLeft) -> {
            referenceSpeed = speed;
            referenceTurn = targetTurn - turnRemaining;
            if (turnRemaining == 0){
                referenceTL = targetTL;
            }else {
                referenceTL = turnLeft;
            }
            Platform.runLater(() -> {
                robotPoint.setVisible(referenceSpeed > 0);
                touchPoint.setVisible(referenceTurn != targetTurn);
                updatePoint(robotPoint, referenceSpeed, referenceTurn, referenceTL);
            });

        });

        robotPoint.setVisible(false);
        touchPoint.setVisible(false);

        touchPoint.setMouseTransparent(true);
        robotPoint.setMouseTransparent(true);

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
                updateSender.sendUpdate(-80,0,true);
            } catch (PortConnectionException e) {
                complete(RCScreenResponse.LostConnection);
            }
        });

        ((Shape) button1.getChildren().get(0)).setFill(disabledFill);

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
            int speed = 0, turnAmount = 0;
            boolean turnLeft = false;
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

            double deg = Math.atan2(y, x) * (180 / Math.PI) + 90;
            if (deg > 180)
                deg = deg - 360;

            if (deg < 0) {
                turnLeft = true;
                deg = -deg;
            } else {
                turnLeft = false;
            }
            turnAmount = (int) ((deg / 180d) * MAX_UNSIGNED_INT);

            if (turnAmount < MINTURN)
                turnAmount = 0;

            if (speed > 0) {
                touchPoint.setVisible(true);
                targetTurn = turnAmount;
                targetSpeed = speed;
                targetTL = turnLeft;
            } else {
                touchPoint.setVisible(false);
                targetTurn = 0;
                targetSpeed = 0;
                targetTL = turnLeft;
            }

            updatePoint(touchPoint, targetSpeed, targetTurn, targetTL);
            try {
                updateSender.sendUpdate((int) targetSpeed, (int) targetTurn, targetTL);
            } catch (PortConnectionException e) {
                complete(RCScreenResponse.LostConnection);
            }

            // System.out.println("X:"+x+" Y:"+y+" s:"+speed+" tA:"+turnAmount+" tL:"+turnLeft);
        });

        // circleImage.setOnTouchMoved(circleImage.getOnTouchPressed());


        InvalidationListener sizeListener = observable -> {
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
            touchPoint.setRadius(circleImage.getFitWidth() / 14.0);
            robotPoint.setRadius(circleImage.getFitWidth() / 16.0);
        };

        background.widthProperty().addListener(sizeListener);
        background.heightProperty().addListener(sizeListener);

    }

    private void updatePoint(Circle touchPoint, double speed, double angle, boolean turnLeft) {
        double x, y;
        angle = (angle / MAX_UNSIGNED_INT) * Math.PI;
        if (turnLeft) {
            angle = -angle;
        }
        angle = angle - Math.PI / 2;
        speed = speed / 255.0;

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
