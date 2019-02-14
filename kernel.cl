__kernel void test(__global size_t* a){
    size_t i = get_global_id(0);
    a[i] = get_group_id(0) + 1;
}