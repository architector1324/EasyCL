typedef struct{
    float x, y, z;
} Vertex;

__kernel void simd(__global int* a){
    size_t i = get_global_id(0);
    a[i] = (int)get_group_id(0) + 1;
}

__kernel void single(__global int* a){
    *a += 2;
}

__kernel void _struct(__global Vertex* v){
    v->x = 1;
    v->y = 2;
    v->z = 3;
}

__kernel void add(__global float* v1, __global float* v2, __global float* result){
    size_t i = get_global_id(0);
	result[i] = v1[i];
}