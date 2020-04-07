#include <stdio.h>
#include "vc.h"

int main(void)
{
    IVC *image;
    IVC *rgb_result, *gray_result;

    image = vc_read_image("Images/Additional/tiffany.ppm");
    rgb_result = vc_image_new(image->width, image->height, 3, 255);
    gray_result = vc_image_new(image->width, image->height, 1, 255);

    if (image == NULL)
    {
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }

    vc_image_darker(image, rgb_result, 70);
    vc_write_image("rgb_darker.pbm", rgb_result);

    vc_image_lighter(image, rgb_result, 10);
    vc_write_image("rgb_lighter.pbm", rgb_result);

    vc_image_free(image);
    vc_image_free(gray_result);
    vc_image_free(rgb_result);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}