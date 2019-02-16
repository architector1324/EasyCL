__kernel void test(__global size_t* a, __constant size_t* thread_id){
    size_t i = *thread_id;
    a[i] = i + 1;
}