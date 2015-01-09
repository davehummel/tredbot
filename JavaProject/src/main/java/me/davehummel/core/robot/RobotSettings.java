package me.davehummel.core.robot;

import me.davehummel.core.providers.connection.PortConnectionException;
import me.davehummel.core.providers.localstore.LocalStoreProvider;


/**
 * Created by davidhummel on 12/16/14.
 */
public class RobotSettings {


    private boolean isCustom;

    public enum RobotSettingsFields {
        STEP_INTERVAL,SPEED_DELTA_PER_INTERVAL,ANGLE_PER_INT_STOPPED,ANGLE_PER_INT_FAST,TRED_DELTA_STOPPED,TRED_DELTA_FAST
    };

    final private int stepInterval,speedDeltaPerInterval,defTredDeltaStopped,defTredDeltaFast;
    final private float expAnglePerIntervalStopped,expAnglePerIntervalFast;

    public RobotSettings(int stepInterval,int speedDeltaPerInterval, float expAnglePerIntervalStopped, float expAnglePerIntervalFast, int defTredDeltaStopped, int defTredDeltaFast) {
        this.stepInterval = stepInterval;
        this.speedDeltaPerInterval = speedDeltaPerInterval;
        this.expAnglePerIntervalStopped = expAnglePerIntervalStopped;
        this.expAnglePerIntervalFast = expAnglePerIntervalFast;
        this.defTredDeltaStopped = defTredDeltaStopped;
        this.defTredDeltaFast = defTredDeltaFast;
        this.isCustom = true;
    }

    public RobotSettings(LocalStoreProvider lsp) {
        this.isCustom = true;
        int temp ;
        try{
            temp = Integer.parseInt(lsp.get(RobotSettingsFields.STEP_INTERVAL.name()));
        } catch (Exception e){
            temp = -1;
            this.isCustom = false;
        }
        stepInterval = temp;
        try{
             temp = Integer.parseInt(lsp.get(RobotSettingsFields.SPEED_DELTA_PER_INTERVAL.name()));
        } catch (Exception e){
            temp = -1;
            this.isCustom = false;
        }
        speedDeltaPerInterval = temp;

        float tempF;
        try{
            tempF = Float.parseFloat(lsp.get(RobotSettingsFields.ANGLE_PER_INT_STOPPED.name()));
        } catch (Exception e){
            tempF = -1;
            this.isCustom = false;
        }
        expAnglePerIntervalStopped = tempF;
        try{
            tempF = Float.parseFloat(lsp.get(RobotSettingsFields.ANGLE_PER_INT_FAST.name()));
        } catch (Exception e){
            tempF = -1;
            this.isCustom = false;
        }
        expAnglePerIntervalFast = tempF;
        try{
            temp = Integer.parseInt(lsp.get(RobotSettingsFields.TRED_DELTA_STOPPED.name()));
        } catch (Exception e){
            temp = -1;
            this.isCustom = false;
        }
        defTredDeltaStopped = temp;
        try{
            temp = Integer.parseInt(lsp.get(RobotSettingsFields.TRED_DELTA_FAST.name()));
        } catch (Exception e){
            temp = -1;
            this.isCustom = false;
        }
        defTredDeltaFast = temp;
    }

    public RobotSettings(RobotConnection connection) throws PortConnectionException {
        String response = connection.ask(RobotConnection.GET_CONFIG_COMMAND,null);
        String[] values = response.split(" ");
        int temp ;

        try{
            isCustom = Boolean.parseBoolean(values[0]);
        }catch (Exception e){
            isCustom = false;
        }

        try{
            temp = Integer.parseInt(values[1]);
        }catch (Exception e){
            temp = -1;
        }
        stepInterval = temp;

        try{
            temp = Integer.parseInt(values[2]);
        }catch (Exception e){
            temp = -1;
        }
        speedDeltaPerInterval = temp;

        float tempF;
        try{
            tempF = Float.parseFloat(values[3]);
        }catch (Exception e){
            tempF = -1;
        }
        expAnglePerIntervalStopped = tempF;

        try{
            tempF = Float.parseFloat(values[4]);
        }catch (Exception e){
            tempF = -1;
        }
        expAnglePerIntervalFast = tempF;

        try{
            temp = Integer.parseInt(values[5]);
        }catch (Exception e){
            temp = -1;
        }
        defTredDeltaStopped = temp;

        try{
            temp = Integer.parseInt(values[6]);
        }catch (Exception e){
            temp = -1;
        }
        defTredDeltaFast = temp;
    }

    public void writeToRobot(RobotConnection connection) throws PortConnectionException {
        StringBuilder message = new StringBuilder();
        message.append(Integer.toString(stepInterval));
        message.append(' ');
        message.append(Integer.toString(speedDeltaPerInterval));
        message.append(' ');
        message.append(Float.toString(expAnglePerIntervalStopped));
        message.append(' ');
        message.append(Float.toString(expAnglePerIntervalFast));
        message.append(' ');
        message.append(Integer.toString(defTredDeltaStopped));
        message.append(' ');
        message.append(Integer.toString(defTredDeltaFast));
        message.append(' ');
        connection.ask(RobotConnection.CONFIG_COMMAND,message.toString());
    }

    public void writeToStore(LocalStoreProvider lsp){
        lsp.set(RobotSettingsFields.STEP_INTERVAL.name(),Integer.toString(stepInterval));
        lsp.set(RobotSettingsFields.SPEED_DELTA_PER_INTERVAL.name(),Integer.toString(speedDeltaPerInterval));
        lsp.set(RobotSettingsFields.ANGLE_PER_INT_STOPPED.name(),Float.toString(expAnglePerIntervalStopped));
        lsp.set(RobotSettingsFields.ANGLE_PER_INT_FAST.name(),Float.toString(expAnglePerIntervalFast));
        lsp.set(RobotSettingsFields.TRED_DELTA_STOPPED.name(),Integer.toString(defTredDeltaStopped));
        lsp.set(RobotSettingsFields.TRED_DELTA_FAST.name(),Integer.toString(defTredDeltaFast));
    }

    public boolean isCustom(){
        return isCustom;
    }

    public int getStepInterval() {
        return stepInterval;
    }

    public int getSpeedDeltaPerInterval() {
        return speedDeltaPerInterval;
    }

    public float getExpAnglePerIntervalStopped() {
        return expAnglePerIntervalStopped;
    }

    public float getExpAnglePerIntervalFast() {
        return expAnglePerIntervalFast;
    }

    public int getDefTredDeltaStopped() {
        return defTredDeltaStopped;
    }

    public int getDefTredDeltaFast() {
        return defTredDeltaFast;
    }
}
