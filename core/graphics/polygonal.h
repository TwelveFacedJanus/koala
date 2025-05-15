#ifndef KOALA_POLYGONAL_H
#define KOALA_POLYGONAL_H

#include "../types.h"
#include "../math/mathematica.h"

/**
 * @defgroup Polygonal Полигоны
 * @brief Логика работы с 3D/2D-типами.
 */

 /**
  * @defgroup 2D/3D структуры.
  * @ingroup Polygonal
  */
typedef struct Vertex2D {
    float x;
    float y;
} Vertex2D;


typedef struct Vertex3D {
    float x;
    float y;
    float z;
} Vertex3D;

typedef Vertex3D Vector3f;
typedef Vertex2D Vector2f;

typedef struct Triangle3D {
    Vertex3D    v1[3];
    Vertex3D    v2[3];
    Vertex3D    v3[3];
} Trinagle3D;

typedef struct Triangle2D {
    Vertex2D    v1[2];
    Vertex2D    v2[2];
    Vertex2D    v3[2];
}  Triangle2D;


typedef struct Object3D {
    Triangle3D* triangles;
    Vector3f pos;
} Object3D;

typedef struct Object2D {
    Triangle2D triangles[2];
    Vector3f pos;
} Object2D;

typedef Object2D Rect;
typedef Object3D Cube;





#endif // KOALA_POLYGONAL_H