#define PI 3.14159f

__kernel void copy_image(__global const uchar4 *imageInput,
                         __global       uchar4 *imageOutput,
                         int width, int height, int N_filtre) 
{
    int2 coord = (int2)(get_global_id(0), get_global_id(1));

    if (coord.x < width && coord.y < height) {
        float4 sum = (float4)(0.0f);

        float denom = 0.0;
        float sigma=2.0f;
        float sigmasq=sigma*sigma;
        

        for (int i = 0; i <= N_filtre; i++) {
            for (int j = 0; j <= N_filtre; j++) {
                int nxp = coord.x + i;
                int nyp = coord.y + j;

                int nxm =coord.x - i;
                int nym = coord.y - j;
                int distancesq=i*i+j*j;
                float localVal = (1.0f/(sqrt(2*PI)*sigma)) * exp(-distancesq/(2*sigmasq));

                if (nxp >= 0 && nxp < width && nyp >= 0 && nyp < height) {
                    int idx = nyp * width + nxp;
                    uchar4 pixel = imageInput[idx];
                    sum += localVal*convert_float4(pixel);
                    denom+=localVal;
                }
                if (nxm >= 0 && nxm < width && nym >= 0 && nym < height) {
                    int idx = nym * width + nxm;
                    uchar4 pixel = imageInput[idx];
                    sum += localVal*convert_float4(pixel);
                    denom+=localVal;
                }
                if (nxp >= 0 && nxp < width && nym >= 0 && nym < height) {
                    int idx = nym * width + nxp;
                    uchar4 pixel = imageInput[idx];
                    sum += localVal*convert_float4(pixel);
                    denom+=localVal;
                }
                if (nxm >= 0 && nxm < width && nyp >= 0 && nyp < height) {
                    int idx = nyp * width + nxm;
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