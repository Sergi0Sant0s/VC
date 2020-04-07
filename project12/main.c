#include <stdio.h>
#include "vc.h"

int main(void)
{
    IVC *image;
    IVC *rgb_result, *gray_result;

    image = vc_read_image("img_seg/coins.pgm");
    gray_result = vc_image_new(image->width, image->height, 1, 255);
    rgb_result = vc_image_new(image->width, image->height, 3, 255);
    

    if (image == NULL)
    {
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }

    vc_rgb_gray_to_binary_global_mean(image, gray_result);
    vc_write_image("gray_to_rgb_mean.ppm",gray_result);
    
    vc_rgb_gray_to_binary(image, gray_result,155);
    vc_write_image("gray_to_rgb_fixed.ppm",gray_result);
    
    vc_rgb_threshold(image, gray_result, 107, 108, 54);
    vc_write_image("teste.ppm",gray_result);
    
    vc_image_free(image);
    vc_image_free(gray_result);
    vc_image_free(rgb_result);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}