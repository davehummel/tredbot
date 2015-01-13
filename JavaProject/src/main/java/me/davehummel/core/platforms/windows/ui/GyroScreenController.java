package me.davehummel.core.platforms.windows.ui;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import me.davehummel.core.platforms.windows.dof.DOFGroup;
import me.davehummel.core.platforms.windows.dof.DOFSensor;
import me.davehummel.core.platforms.windows.dof.HeadingGroup;
import me.davehummel.core.robot.LineReceiver;

import java.net.URL;
import java.util.ResourceBundle;

/**
 * Created by Dave on 12/21/2014.
 */
public class GyroScreenController extends UIController implements Initializable {


    @FXML
    private TextArea lineText;

    @FXML
    private TextField mxText;
    @FXML
    private TextField myText;
    @FXML
    private TextField mzText;
    @FXML
    private TextField axText;
    @FXML
    private TextField ayText;
    @FXML
    private TextField azText;
    @FXML
    private TextField gxText;
    @FXML
    private TextField gyText;
    @FXML
    private TextField gzText;
    @FXML
    private TextField tempText;
    @FXML
    private TextField msText;
    @FXML
    private ToggleButton readyButton;
    @FXML
    private Button resetButton;
    @FXML
    private Button revertButton;
    @FXML
    private RadioButton currentOption;
    @FXML
    private RadioButton zAvgOption;
    @FXML
    private RadioButton zMinOption;
    @FXML
    private RadioButton zMaxOption;
    @FXML
    private RadioButton aAvgOption;
    @FXML
    private RadioButton aMinOption;
    @FXML
    private RadioButton aMaxOption;
    @FXML
    private ToggleGroup displayMode;
    @FXML
    private TextField magHText;
    @FXML
    private TextField calcMagHText;
    @FXML
    private TextField gyroHText;
    @FXML
    private TextField estVelText;
    @FXML
    private TextField gyroDeadZoneXText;
    @FXML
    private TextField gyroDeadZoneYText;
    @FXML
    private TextField gyroDeadZoneZText;
    @FXML
    private TextField acclDeadZoneXText;
    @FXML
    private TextField acclDeadZoneYText;
    @FXML
    private TextField acclDeadZoneZText;


    private LineReceiver lineReceiver;
    private DOFSensor sensor = new DOFSensor();
    private long lastMeasureTime = -1;

    @Override
    public void initialize(URL location, ResourceBundle resources) {

        readyButton.setOnAction(event -> {
            sensor.ready();
        });
        resetButton.setOnAction(event -> {
            sensor.resetRunningStats();
        });
        revertButton.setOnAction(event -> {
            sensor.revertGyroHeading();
        });

        lineReceiver.setListener(line -> {
            Platform.runLater( ()-> {
                lineText.setText(line);
                float mx,my,mz,ax,ay,az,gx,gy,gz,t,mhx,mhy,mhz;
                String[] vals = line.split(":");
                float temp = -1;
                try{
                    temp = Float.parseFloat(vals[1]);
                }catch (Exception e){
                    temp=-1;
                }
                ax = temp;
                try{
                    temp = Float.parseFloat(vals[2]);
                }catch (Exception e){
                    temp=-1;
                }
                ay = temp;
                try{
                    temp = Float.parseFloat(vals[3]);
                }catch (Exception e){
                    temp=-1;
                }
                az = temp;
                try{
                    temp = Float.parseFloat(vals[5]);
                }catch (Exception e){
                    temp=-1;
                }
                mx = temp;
                try{
                    temp = Float.parseFloat(vals[6]);
                }catch (Exception e){
                    temp=-1;
                }
                my = temp;
                try{
                    temp = Float.parseFloat(vals[7]);
                }catch (Exception e){
                    temp=-1;
                }
                mz = temp;
                try{
                    temp = Float.parseFloat(vals[9]);
                }catch (Exception e){
                    temp=-1;
                }
                gx = temp;
                try{
                    temp = Float.parseFloat(vals[10]);
                }catch (Exception e){
                    temp=-1;
                }
                gy = temp;
                try{
                    temp = Float.parseFloat(vals[11]);
                }catch (Exception e){
                    temp=-1;
                }
                gz = temp;
                try{
                    temp = Float.parseFloat(vals[13]);
                }catch (Exception e){
                    temp=-1;
                }
                t = temp;
                try{
                    temp = Float.parseFloat(vals[15]);
                }catch (Exception e){
                    temp=-1;
                }
                mhx = temp;
                try{
                    temp = Float.parseFloat(vals[16]);
                }catch (Exception e){
                    temp=-1;
                }
                mhy = temp;
                try{
                    temp = Float.parseFloat(vals[17]);
                }catch (Exception e){
                    temp=-1;
                }
                mhz = temp;
                HeadingGroup mheading = new HeadingGroup(mhx,mhy,mhz);

                long measureTime = -1;
                try{
                    measureTime = Long.parseLong(vals[0]);
                }catch (Exception e){

                }

                DOFGroup group = new DOFGroup(mx,my,mz,ax,ay,az,gx,gy,gz,t);

                int interval = 0;
                if (lastMeasureTime == -1 || measureTime == -1){
                    interval = 0;
                }else{
                    interval = (int) (measureTime - lastMeasureTime);
                }
                msText.setText(Integer.toString(interval));
                lastMeasureTime = measureTime;
                sensor.update(interval,group);
                sensor.setMagneticHeading(mheading);
                if (currentOption.isSelected()){
                    group = sensor.getCurrent();
                }else if (zAvgOption.isSelected()){
                    group = sensor.getZeroAvg();
                }else if (zMinOption.isSelected()){
                    group = sensor.getZeroMins();
                }else if (zMaxOption.isSelected()){
                    group = sensor.getZeroMaxs();
                }else if (aAvgOption.isSelected()){
                    group = sensor.getAllAvg();
                }else if (aMaxOption.isSelected()){
                    group = sensor.getAllMaxs();
                }else if (aMinOption.isSelected()){
                    group = sensor.getAllMins();
                }

                mxText.setText(Float.toString(group.mx));
                myText.setText(Float.toString(group.my));
                mzText.setText(Float.toString(group.mz));

                axText.setText(Float.toString(group.ax));
                ayText.setText(Float.toString(group.ay));
                azText.setText(Float.toString(group.az));

                gxText.setText(Float.toString(group.gx));
                gyText.setText(Float.toString(group.gy));
                gzText.setText(Float.toString(group.gz));

                tempText.setText(Float.toString(group.t));

                magHText.setText(Float.toString(mheading.z));
                calcMagHText.setText(Float.toString(sensor.getMagneticHeading().z));
                gyroHText.setText(Float.toString(sensor.getGyroHeading().z));

                acclDeadZoneXText.setPromptText(Float.toString((sensor.getZeroMaxs().ax-sensor.getZeroMins().ax)));
                acclDeadZoneYText.setPromptText(Float.toString((sensor.getZeroMaxs().ay-sensor.getZeroMins().ay)));
                acclDeadZoneZText.setPromptText(Float.toString((sensor.getZeroMaxs().az-sensor.getZeroMins().az)));

                gyroDeadZoneXText.setPromptText(Float.toString((sensor.getZeroMaxs().gx-sensor.getZeroMins().gx)));
                gyroDeadZoneYText.setPromptText(Float.toString((sensor.getZeroMaxs().gy-sensor.getZeroMins().gy)));
                gyroDeadZoneZText.setPromptText(Float.toString((sensor.getZeroMaxs().gz-sensor.getZeroMins().gz)));

                acclDeadZoneXText.textProperty().addListener((observable, oldValue, newValue) -> {
                    if (newValue.isEmpty())
                        newValue = "0";
                    try{
                        sensor.setAcclDeadZoneX(Float.parseFloat(newValue));
                    }catch(Exception e){

                    }
                });
                acclDeadZoneYText.textProperty().addListener((observable, oldValue, newValue) -> {
                    if (newValue.isEmpty())
                        newValue = "0";
                    try{
                        sensor.setAcclDeadZoneY(Float.parseFloat(newValue));
                    }catch(Exception e){

                    }
                });
                acclDeadZoneZText.textProperty().addListener((observable, oldValue, newValue) -> {
                    if (newValue.isEmpty())
                        newValue = "0";
                    try{
                        sensor.setAcclDeadZoneZ(Float.parseFloat(newValue));
                    }catch(Exception e){

                    }
                });

                gyroDeadZoneXText.textProperty().addListener((observable, oldValue, newValue) -> {
                    if (newValue.isEmpty())
                        newValue = "0";
                    try{
                        sensor.setGyroDeadZoneX(Float.parseFloat(newValue));
                    }catch(Exception e){

                    }
                });
                gyroDeadZoneYText.textProperty().addListener((observable, oldValue, newValue) -> {
                    if (newValue.isEmpty())
                        newValue = "0";
                    try{
                        sensor.setGyroDeadZoneY(Float.parseFloat(newValue));
                    }catch(Exception e){

                    }
                });
                gyroDeadZoneZText.textProperty().addListener((observable, oldValue, newValue) -> {
                    if (newValue.isEmpty())
                        newValue = "0";
                    try{
                        sensor.setGyroDeadZoneZ(Float.parseFloat(newValue));
                    }catch(Exception e){

                    }
                });

            });
        });
    }


    public void setInitialValues(LineReceiver lineReceiver) {
        this.lineReceiver = lineReceiver;
    }
}
