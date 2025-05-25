#define PI 3.14159f

__kernel void copy_image(__global const uchar4 *imageInput,
                         __global       uchar4 *imageOutput,
                         int width, int height, int N_filtre) 
{
    int2 coord = (int2)(get_global_id(0), get_global_id(1));

    if (coord.x < width && coord.y < height) {
        float4 sum = (float4)(0.0f);

        int nb_of_pixel = 0;

        for (int i = -N_filtre; i <= N_filtre; i++) {
            for (int j = -N_filtre; j <= N_filtre; j++) {
                int nx = coord.x + i;
                int ny = coord.y + j;

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int idx = ny * width + nx;
                    uchar4 pixel = imageInput[idx];
                    sum += convert_float4(pixel);
                    nb_of_pixel++;
                }
            }
        }

        // Moyenne des pixels du filtre
        sum = sum / (float)nb_of_pixel;

        uchar4 result = convert_uchar4_rte(sum);

        int out_idx = coord.y * width + coord.x;
        imageOutput[out_idx] = result;
    }
}