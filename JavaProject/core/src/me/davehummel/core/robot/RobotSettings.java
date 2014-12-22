package me.davehummel.core.robot;

import me.davehummel.core.providers.localstore.LocalStoreProvider;

import java.io.IOException;


/**
 * Created by davidhummel on 12/16/14.
 */
public class RobotSettings {


    public enum RobotSettingsFields {
        STEP_INTERVAL,SPEED_DELTA_PER_INTERVAL,STOPPED_TURN_TIME,FAST_TURN_TIME,STOPPED_TURN_SPEED,FAST_TURN_SPEED
    };

    final private int stepInterval,speedDeltaPerInterval,stoppedTurnTime,fastTurnTime,stoppedTurnSpeed,fastTurnSpeed;

    public RobotSettings(int stepInterval,int speedDeltaPerInterval, int stoppedTurnTime, int fastTurnTime, int stoppedTurnSpeed, int fastTurnSpeed) {
        this.stepInterval = stepInterval;
        this.speedDeltaPerInterval = speedDeltaPerInterval;
        this.stoppedTurnTime = stoppedTurnTime;
        this.fastTurnTime = fastTurnTime;
        this.stoppedTurnSpeed = stoppedTurnSpeed;
        this.fastTurnSpeed = fastTurnSpeed;
    }

    public RobotSettings(LocalStoreProvider lsp) {

        int temp ;
        try{
            temp = Integer.parseInt(lsp.get(RobotSettingsFields.STEP_INTERVAL.name()));
        } catch (Exception e){
            temp = -1;
        }
        stepInterval = temp;


        try{
             temp = Integer.parseInt(lsp.get(RobotSettingsFields.SPEED_DELTA_PER_INTERVAL.name()));
        } catch (Exception e){
            temp = -1;
        }
        speedDeltaPerInterval = temp;


        try{
            temp = Integer.parseInt(lsp.get(RobotSettingsFields.STOPPED_TURN_TIME.name()));
        } catch (Exception e){
            temp = -1;
        }
        stoppedTurnTime = temp;


        try{
            temp = Integer.parseInt(lsp.get(RobotSettingsFields.FAST_TURN_TIME.name()));
        } catch (Exception e){
            temp = -1;
        }
        fastTurnTime = temp;


        try{
            temp = Integer.parseInt(lsp.get(RobotSettingsFields.STOPPED_TURN_SPEED.name()));
        } catch (Exception e){
            temp = -1;
        }
        stoppedTurnSpeed = temp;


        try{
            temp = Integer.parseInt(lsp.get(RobotSettingsFields.FAST_TURN_SPEED.name()));
        } catch (Exception e){
            temp = -1;
        }
        fastTurnSpeed = temp;
    }

    public RobotSettings(RobotConnection connection) throws IOException {
        String response = connection.ask(RobotConnection.GET_CONFIG_COMMAND,null);
        String[] values = response.split(" ");
        int temp ;

        try{
            temp = Integer.parseInt(values[0]);
        }catch (Exception e){
            temp = -1;
        }
        stepInterval = temp;

        try{
            temp = Integer.parseInt(values[1]);
        }catch (Exception e){
            temp = -1;
        }
        speedDeltaPerInterval = temp;

        try{
            temp = Integer.parseInt(values[1]);
        }catch (Exception e){
            temp = -1;
        }
        stoppedTurnTime = temp;

        try{
            temp = Integer.parseInt(values[1]);
        }catch (Exception e){
            temp = -1;
        }
        fastTurnTime = temp;

        try{
            temp = Integer.parseInt(values[1]);
        }catch (Exception e){
            temp = -1;
        }
        stoppedTurnSpeed = temp;

        try{
            temp = Integer.parseInt(values[1]);
        }catch (Exception e){
            temp = -1;
        }
        fastTurnSpeed = temp;
    }

    public void writeToRobot(RobotConnection connection) throws IOException {
        StringBuilder message = new StringBuilder();
        message.append(Integer.toString(stepInterval));
        message.append(' ');
        message.append(Integer.toString(speedDeltaPerInterval));
        message.append(' ');
        message.append(Integer.toString(stoppedTurnTime));
        message.append(' ');
        message.append(Integer.toString(fastTurnTime));
        message.append(' ');
        message.append(Integer.toString(stoppedTurnSpeed));
        message.append(' ');
        message.append(Integer.toString(fastTurnSpeed));
        message.append(' ');
        connection.ask(RobotConnection.CONFIG_COMMAND,message.toString());
    }

    public void writeToStore(LocalStoreProvider lsp){
        lsp.set(RobotSettingsFields.STEP_INTERVAL.name(),Integer.toString(stepInterval));
        lsp.set(RobotSettingsFields.SPEED_DELTA_PER_INTERVAL.name(),Integer.toString(speedDeltaPerInterval));
        lsp.set(RobotSettingsFields.STOPPED_TURN_TIME.name(),Integer.toString(stoppedTurnTime));
        lsp.set(RobotSettingsFields.FAST_TURN_TIME.name(),Integer.toString(fastTurnTime));
        lsp.set(RobotSettingsFields.STOPPED_TURN_SPEED.name(),Integer.toString(stoppedTurnSpeed));
        lsp.set(RobotSettingsFields.FAST_TURN_SPEED.name(),Integer.toString(fastTurnSpeed));
    }

}
