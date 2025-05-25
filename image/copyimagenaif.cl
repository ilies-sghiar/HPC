__kernel void copy_image(__global const uchar *imageInput,
                         __global uchar *imageOutput,
                         int width, int height, int N)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= width || y >= height) return;

    int sumR = 0;
    int sumG = 0;
    int sumB = 0;
    int sumA = 0;
    int count = 0;

    // Parcourir un voisinage autour du pixel (x, y)
    for (int dy = -N; dy <= N; dy++) {
        for (int dx = -N; dx <= N; dx++) {
            int nx = x + dx;
            int ny = y + dy;

            // Vérifier que le voisin est dans l'image
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int nindex = (ny * width + nx) * 4;
                sumR += imageInput[nindex];
                sumG += imageInput[nindex + 1];
                sumB += imageInput[nindex + 2];
                sumA += imageInput[nindex + 3];
                count++;
            }
        }
    }

    int index = (y * width + x) * 4;

    // Calculer la moyenne
    imageOutput[index]     = (uchar)(sumR / count);
    imageOutput[index + 1] = (uchar)(sumG / count);
    imageOutput[index + 2] = (uchar)(sumB / count);
    imageOutput[index + 3] = (uchar)(sumA / count);
}
