package me.davehummel.core.platforms.windows;

import me.davehummel.core.providers.localstore.LocalStoreProvider;

import java.io.*;
import java.util.Date;
import java.util.Properties;

/**
 * Created by Dave on 12/21/2014.
 */
public class WindowsLocalStoreProvider implements LocalStoreProvider {

    Properties properties = new Properties();

    public WindowsLocalStoreProvider() {
        FileInputStream in = null;
        try {
            File file = new File("resources/properties.txt");
            System.out.println("Loading properties from:"+file.getAbsolutePath());
            in = new FileInputStream(file);
            properties.load(in);
        } catch (Exception e) {
            System.err.println("Unable to load properties!");
            e.printStackTrace();
        } finally {
            try {
                if (in != null)
                    in.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    @Override
    public void set(String key, String val) {
        properties.setProperty(key, val);
    }

    @Override
    public String get(String key) {
        return properties.getProperty(key);
    }

    @Override
    public void remove(String key) {
        properties.remove(key);
    }

    @Override
    public void persist() {
        FileOutputStream out = null;
        try {
            out = new FileOutputStream("appProperties");
            properties.store(out, new Date().toString());
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (out != null)
                    out.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    @Override
    public void shutdown() {

    }
}
