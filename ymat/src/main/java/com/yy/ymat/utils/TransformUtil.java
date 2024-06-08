package com.yy.ymat.utils;

public class TransformUtil {

    public static float linear(int cur, int start, int end) {
        return (float) (cur - start) / (float) (end - start);
    }

    public static float easeInQuad(int cur, int start, int end) {
        final float fas = (float) (cur - start) / (float) (end - start);
        return fas * fas;
    }

    public static float easeOutQuad(int cur, int start, int end) {
        final float fas = (float) (cur - start) / (float) (end - start);
        return -fas * (fas - 2);
    }

    public static float easeInOutQuad(int cur, int start, int end) {
        float fas = (float) (cur - start) * 2 / (float) (end - start);
        if (fas < 1) {
            return (float) (fas * fas * 0.5);
        }
        fas--;
        return (float) (-0.5 * (fas * (fas - 2) - 1));
    }
}
