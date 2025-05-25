__kernel void saxpy_gpu(__global float *A, __global float *B, __global float *C, __global float *a) {
	// Get the index of the current element
	int i = get_global_id(0);
	// Do the operation
	C[i] = A[i]* a[0] + B[i];
}
