#ifndef RAY_SLAB_H
#define RAY_SLAB_H

#include <limits>
#include <algorithm>
template<typename T,int I>
inline static void slab_onedim(T rayO[3],T rayN[3],T boxMin[3],T boxMax[3],T Tnear[3],T Tfar[3]){
    if(rayN[I]==0){
        if(rayO[I] < boxMax[I] && rayO[I] > boxMin[I]){
            Tnear[I]=std::numeric_limits<T>::min();
            Tfar[I]=std::numeric_limits<T>::max();
        }else{
            Tnear[I]=std::numeric_limits<T>::max();
            Tfar[I]=std::numeric_limits<T>::max();
        }
    }else{
        Tnear[I]=(boxMin[I]-rayO[I]) / rayN[I];
        Tfar[I]=(boxMax[I]-rayO[I]) / rayN[I];
        if(Tnear[I]>Tfar[I]) std::swap(Tnear[I],Tfar[I]);
    }
}

template<typename T>
bool slab_test_3d(T rayO[3],T rayN[3],T boxMin[3],T boxMax[3]){
    T Tnear[3],Tfar[3];
    slab_onedim<T,0>(rayO,rayN,boxMin,boxMax,Tnear,Tfar);
    slab_onedim<T,1>(rayO,rayN,boxMin,boxMax,Tnear,Tfar);
    slab_onedim<T,2>(rayO,rayN,boxMin,boxMax,Tnear,Tfar);
    if(std::max(Tnear[0],std::max(Tnear[1],Tnear[2])) < std::min(Tfar[0],std::min(Tfar[1],Tfar[2])) )
        return true;
    else
        return false;
}


#endif // RAY_SLAB_H
