#ifndef _PFC_ENDIAN_CHANGE_H
#define _PFC_ENDIAN_CHANGE_H

#include <algorithm>

using namespace std;

template <class T>
void endswap(T *objp){
    unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
    reverse(memp, memp + sizeof(T));
}

#endif  // _PFC_ENDIAN_CHANGE_H
