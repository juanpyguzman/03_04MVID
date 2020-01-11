#ifndef __CUBEWRAPPED_H__
#define __CUBEWRAPPED_H__ 1

#include "geometry.hpp"

class CubeWrapped final : public Geometry {
    public:
        CubeWrapped() = delete;
        explicit CubeWrapped(float size);

    private:
        float _size;
};
#endif