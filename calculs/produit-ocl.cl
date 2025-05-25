__kernel void produit_gpu(__global const float* A, 
                         __global const float* B, 
                         __global float* C,
                         const int DIM) 
{
    int row = get_global_id(0);
    int col = get_global_id(1);

    if (row < DIM && col < DIM) {
        float sum = 0.0f;
        for (int k = 0; k < DIM; k++) {
            sum += A[row * DIM + k] * B[k * DIM + col];
        }
        C[row * DIM + col] = sum;
    }
}

