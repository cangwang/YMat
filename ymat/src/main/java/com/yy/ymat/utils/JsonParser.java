package com.yy.ymat.utils;

import android.os.SystemClock;
import android.text.TextUtils;
import android.util.Log;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonPrimitive;
import com.google.gson.JsonSerializationContext;
import com.google.gson.JsonSerializer;
import com.google.gson.internal.Primitives;
import com.google.gson.reflect.TypeToken;

import java.lang.reflect.Array;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Auto parse json string to object
 *
 * @author <a href="mailto:kuanglingxuan@yy.com">匡凌轩</a> 2014-6-10
 * Created by zhongyongsheng on 14-4-11.
 */
public class JsonParser {

    private static final String JSON_OBJECT_DEFAULT_STR = "{}";
    private static final String TAG = "JsonParser";
    public static Gson gson = new GsonBuilder()
            .registerTypeAdapter(new TypeToken<Map<Object, Object>>() {
            }.getType(), new MapTypeAdapter())
            .registerTypeAdapter(new TypeToken<Map<String, Object>>() {
            }.getType(), new MapTypeAdapter())
            // .registerTypeAdapter(new TypeToken<Map<Integer, Object>>() {
            // }.getType(), new MapTypeAdapter())
            // .registerTypeAdapter(new TypeToken<Map<Long, Object>>() {
            // }.getType(), new MapTypeAdapter())
            // .registerTypeAdapter(new TypeToken<Map<Double, Object>>() {
            // }.getType(), new MapTypeAdapter())
            .disableHtmlEscaping() //Gson将对象类转成Json对象时 = 出现\u003d的问题
            .create();

    /**
     * parse json string to object
     *
     * @param json
     * @param <T>
     * @return
     * @throws java.io.IOException
     */
    public static <T> T parseJsonObject(String json, Class<T> clz) {
        long t = SystemClock.currentThreadTimeMillis();
        try {
            return gson.fromJson(json, clz);
        } catch (Throwable throwable) {
            Log.e(TAG, throwable.toString());
        } finally {
        }
        return null;
    }

    public static <T> T parseJsonObject(JsonElement json, Class<T> clz) {
        long t = SystemClock.currentThreadTimeMillis();
        try {
            return gson.fromJson(json, clz);
        } catch (Throwable throwable) {
            Log.e(TAG, throwable.toString());
        }
        return null;
    }

    public static <T> T parseJsonObject(String json, Type type) {
        long t = SystemClock.currentThreadTimeMillis();
        try {
            return gson.fromJson(json, type);
        } catch (Throwable throwable) {
            Log.e(TAG, throwable.toString());
        }
        return null;
    }

    public static <T> T parseJsonObject(JsonElement json, Type type) {
        long t = SystemClock.currentThreadTimeMillis();
        try {
            return gson.fromJson(json, type);
        } catch (Throwable throwable) {
            Log.e(TAG, throwable.toString());
        }
        return null;
    }

    /**
     * parse json string to Array
     *
     * @param clz
     * @return
     * @throws Exception
     */
    public static <T> T[] parseJsonArray(String json, Class<T> clz) {
        long t = SystemClock.currentThreadTimeMillis();
        T[] result;
        try {
            Type type = TypeToken.getArray(Primitives.wrap(clz))
                    .getType();
            result = gson.fromJson(json, type);
            return result;
        } catch (Throwable throwable) {
            Log.e(TAG, throwable.toString());
        }
        try {
            result = (T[]) Array.newInstance(Primitives.wrap(clz), 0);
        } catch (Throwable throwable) {
            Log.e(TAG, throwable.toString());
            return null;
        }

        return result;
    }

    /**
     * parse json string to Map
     *
     * @param json
     * @param keyType
     * @param valueType
     * @param <K>
     * @param <V>
     * @return
     */
    public static <K, V> Map<K, V> parseJsonMap(String json, Class<K> keyType, Class<V> valueType) {
        try {
            Type type = TypeToken.getParameterized(Map.class, Primitives.wrap(keyType),
                            Primitives.wrap(valueType))
                    .getType();
            Map<K, V> result = gson.fromJson(json, type);
            return result;
        } catch (Throwable throwable) {
            Log.e(TAG, json, throwable);
        }
        return new HashMap<K, V>();
    }

    public static String toJson(Object obj) {
        try {
            return gson.toJson(obj);
        } catch (Throwable e) {
            Log.e("JsonParser", "toJson", e);
            return JSON_OBJECT_DEFAULT_STR;
        }
    }

    public static class NumberTypeAdapter implements JsonSerializer<Number> {
        @Override
        public JsonElement serialize(Number src, Type typeOfSrc, JsonSerializationContext context) {
            return new JsonPrimitive(src);
        }
    }

    public static <T> List<T> parseJsonList(String json, Class<T> clz) {
        List<T> data = new ArrayList<T>();
        try {
            if (!TextUtils.isEmpty(json)) {
                com.google.gson.JsonParser parser = new com.google.gson.JsonParser();
                JsonElement element = parser.parse(json);
                JsonArray array = element.getAsJsonArray();
                for (JsonElement jo : array) {
                    data.add(gson.fromJson(jo, clz));
                }
            }
        } catch (Throwable throwable) {
            Log.e(TAG, json, throwable);
        }
        return data;
    }

    public static <T> List<T> parseJsonList(JsonArray array, Class<T> clz) {
        List<T> data = new ArrayList<T>();
        try {
            for (JsonElement jo : array) {
                data.add(parseJsonObject(jo, clz));
            }
        } catch (Throwable throwable) {
            Log.e(TAG, throwable.toString());
        }
        return data;
    }

    public static boolean isDefaultJsonObjStr(String json) {
        return JSON_OBJECT_DEFAULT_STR.equals(json);
    }
}
