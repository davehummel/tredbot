package me.davehummel.core.providers.localstore;

import java.util.Map;

/**
 * Created by davidhummel on 12/16/14.
 */
public interface LocalStoreProvider {

    void set(String key,String val);

    String get(String key);

    void remove(String key);

    void persist();

    void shutdown();
}
