package me.davehummel.core.providers.localstore;

import java.util.Map;

/**
 * Created by davidhummel on 12/16/14.
 */
public interface LocalStoreProvider {

    void setAll(Map<String,String> map);

    void set(String key,String val);

    String get(String key);

    void remove(String key);
}
