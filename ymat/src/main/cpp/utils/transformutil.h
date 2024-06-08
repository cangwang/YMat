//
// Created by Cangwang on 2024/4/18.
//

#pragma once

/**
 * 用于插值计算
 */
namespace ymat {
    class TransformUtil {
    public:
        static float linear(int cur, int start, int end) {
            return (float)(cur - start) / (float)(end -start);
        }

        static float easeInQuad(int cur, int start, int end) {
            const float fas = (float)(cur - start) / (float)(end -start);
            return fas * fas;
        }

        static float easeOutQuad(int cur, int start, int end) {
            const float fas = (float)(cur - start) / (float)(end -start);
            return -fas * (fas -2);
        }

        static float easeInOutQuad(int cur, int start, int end) {
            float fas = (float)(cur - start) * 2 / (float)(end -start);
            if (fas < 1) return static_cast<float>(fas * fas * 0.5);
            fas--;
            return static_cast<float>(-0.5 * (fas * (fas - 2) - 1));
        }
    };
}
