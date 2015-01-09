package me.davehummel.core.providers.ui;

/**
 * Created by Dave on 12/20/2014.
 */
public class ConnectionScreenResponse implements ScreenResponse{

    public static final ConnectionScreenResponse REFRESH = new ConnectionScreenResponse(null),EXIT = new ConnectionScreenResponse(null) ;

    public final String portName;

    public final Exception error;

    public final String extra;

    public ConnectionScreenResponse(String portName) {
        this.portName = portName;
        this.error = null;
        this.extra = null;
    }

    public ConnectionScreenResponse(String portName, Exception e){
        this.portName = portName;
        this.error = e;
        this.extra = null;
    }

    public ConnectionScreenResponse(String portName,  String extra){
        this.portName = portName;
        this.error = null;
        this.extra = extra;
    }

}
