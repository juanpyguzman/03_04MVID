#ifndef __CUBEPROCEDURAL_H__
#define __CUBEPROCEDURAL_H__ 1

#include "geometry.hpp"

class CubeProcedural final : public Geometry {
    public:
        CubeProcedural() = delete;
        explicit CubeProcedural(float radius);

    private:
        float _radius;
};
#endif