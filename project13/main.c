#include <stdio.h>
#include "vc.h"

int main(void)
{
    IVC *image;
    IVC *rgb_result, *gray_result, *teste;

    image = vc_read_image("./img_seg/grayscale_chart.pgm");
    gray_result = vc_image_new(image->width, image->height, 1, 255);
    rgb_result = vc_image_new(image->width, image->height, 3, 255);
    teste = vc_image_new(image->width, image->height, 1, 255);
    

    if (image == NULL)
    {
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }

    
    //vc_gray_to_binary_midpoint(image, gray_result,33);
    //vc_rgb_gray_to_binary(image,gray_result,125);
    vc_rgb_gray_to_binary_global_mean(image,gray_result);
    //vc_gray_negative(gray_result);
    //vc_gray_to_binary_bernsen(image, gray_result, 25, 100);
    vc_binary_dilate(gray_result,teste,3);
    vc_write_image("teste.ppm",teste);
    
    vc_image_free(image);
    vc_image_free(gray_result);
    vc_image_free(rgb_result);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}