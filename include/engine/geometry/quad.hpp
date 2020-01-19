#ifndef __QUAD_H__
#define __QUAD_H__ 1

#include "geometry.hpp"

class Quad final: public Geometry {
    public:
        Quad() = delete;
        explicit Quad(float size);
        explicit Quad(float size, uint32_t numbertiles);

    private:
        float _size;
        uint32_t _numbertiles;
};
#endif