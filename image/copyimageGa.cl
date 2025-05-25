
#define PI 3.14159f

__kernel void copy_image(__global const uchar4 *imageInput,
                         __global       uchar4 *imageOutput,
                         int width, int height, int N_filtre) 
{
    int2 coord = (int2)(get_global_id(0), get_global_id(1));

    if (coord.x < width && coord.y < height) {
        float4 sum = (float4)(0.0f);

        float denom = 0.0;
        float sigma=1.0f; //1 10
        float sigmasq=sigma*sigma;
        

        for (int i = -N_filtre; i <= N_filtre; i++) {
            for (int j = -N_filtre; j <= N_filtre; j++) {
                int nx = coord.x + i;
                int ny = coord.y + j;
                int distancesq=i*i+j*j;
                float localVal = (1.0f/(sqrt(2*PI)*sigma)) * exp(-distancesq/(2*sigmasq));

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int idx = ny * width + nx;
                    uchar4 pixel = imageInput[idx];
                    sum += localVal*convert_float4(pixel);
                    denom+=localVal;
                }
            }
        }

        // Moyenne des pixels du filtre
        sum = sum / (float)denom;

        uchar4 result = convert_uchar4_rte(sum);

        int out_idx = coord.y * width + coord.x;
        imageOutput[out_idx] = result;
    }
}