#include <stdio.h>
#include "vc.h"

int main(void)
{
    IVC *image;
    IVC *rgb_result, *gray_result;

    image = vc_read_image("Imagens/HSVTestImage01.ppm");
    rgb_result = vc_image_new(image->width, image->height, 3, 255);
    gray_result = vc_image_new(image->width, image->height, 1, 255);

    if (image == NULL)
    {
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }

    vc_rgb_to_hsv(image,rgb_result);
    vc_write_image("hsv.ppm", rgb_result);
    
    vc_hsv_segmentation(rgb_result,rgb_result, 0, 100, 0, 30, 0, 100);
    vc_write_image("hsv_segmentation.ppm", rgb_result);
    
    //vc_scale_gray_to_rgb(image, rgb_result);
    //vc_write_image("gray_to_rgb.pgm", rgb_result);
    
    vc_image_free(image);
    vc_image_free(gray_result);
    vc_image_free(rgb_result);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}