typedef struct{
    float x, y, z;
} Vertex;

__kernel void test(__global Vertex* v){
    v->x = 1;
    v->y = 2;
    v->z = 3;
}