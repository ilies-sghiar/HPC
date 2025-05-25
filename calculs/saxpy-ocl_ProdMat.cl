__kernel void matmul_gpu(__global float *A, __global float *B, __global float *C, const int n) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i < n && j < n) {
        float s = 0.0f;
        for (int k = 0; k < n; ++k) {
            s += A[i * n + k] * B[k * n + j];
        }
        C[i * n + j] = s;
    }
}