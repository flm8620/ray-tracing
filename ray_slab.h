#pragma once

#include <limits>
#include <algorithm>

template <typename T, int I>
inline static void slab_onedim(T rayO[3], T rayN[3], T boxMin[3], T boxMax[3], T Tnear[3], T Tfar[3]) {
    if (rayN[I] == 0) {
        if (rayO[I] < boxMax[I] && rayO[I] > boxMin[I]) {
            Tnear[I] = std::numeric_limits<T>::min();
            Tfar[I] = std::numeric_limits<T>::max();
        } else {
            Tnear[I] = std::numeric_limits<T>::max();
            Tfar[I] = std::numeric_limits<T>::max();
        }
    } else {
        Tnear[I] = (boxMin[I] - rayO[I]) / rayN[I];
        Tfar[I] = (boxMax[I] - rayO[I]) / rayN[I];
        if (Tnear[I] > Tfar[I])
            std::swap(Tnear[I], Tfar[I]);
    }
}

template <typename T>
bool slab_test_3d(T rayO[3], T rayN[3], T boxMin[3], T boxMax[3]) {
    T Tnear[3], Tfar[3];
    slab_onedim<T, 0>(rayO, rayN, boxMin, boxMax, Tnear, Tfar);
    slab_onedim<T, 1>(rayO, rayN, boxMin, boxMax, Tnear, Tfar);
    slab_onedim<T, 2>(rayO, rayN, boxMin, boxMax, Tnear, Tfar);
    T t_in = std::max(Tnear[0], std::max(Tnear[1], Tnear[2]));
    T t_out = std::min(Tfar[0], std::min(Tfar[1], Tfar[2]));
    if (t_in <= t_out && t_out > 0)
        return true;
    else
        return false;
}

template <typename T>
bool slab_test_3d_info(T rayO[3], T rayN[3], T boxMin[3], T boxMax[3], T &t_in, T &t_out, int &axis_in, int &axis_out) {
    T Tnear[3], Tfar[3];
    slab_onedim<T, 0>(rayO, rayN, boxMin, boxMax, Tnear, Tfar);
    slab_onedim<T, 1>(rayO, rayN, boxMin, boxMax, Tnear, Tfar);
    slab_onedim<T, 2>(rayO, rayN, boxMin, boxMax, Tnear, Tfar);
    t_in = std::max(Tnear[0], std::max(Tnear[1], Tnear[2]));
    t_out = std::min(Tfar[0], std::min(Tfar[1], Tfar[2]));
    if (t_in <= t_out && t_out > 0) {
        float max_near = Tnear[0];
        axis_in = 0;
        if (Tnear[1] > max_near) {
            max_near = Tnear[1];
            axis_in = 1;
        }
        if (Tnear[2] > max_near) {
            max_near = Tnear[2];
            axis_in = 2;
        }

        float min_far = Tfar[0];
        axis_out = 0;
        if (Tfar[1] < min_far) {
            min_far = Tfar[1];
            axis_out = 1;
        }
        if (Tfar[2] < min_far) {
            min_far = Tfar[2];
            axis_out = 2;
        }
        return true;
    } else
        return false;
}
