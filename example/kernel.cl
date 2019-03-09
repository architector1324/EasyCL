__kernel void test(__global int* a){
    size_t i = get_global_id(0);
    a[i] = (int)get_group_id(0) + 1;
}