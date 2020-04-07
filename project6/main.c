#include <stdio.h>
#include "vc.h"

int main()
{

    IVC *image;
    int x, y;
    long int pos;

    image = vc_image_new(256, 256, 1, 255);

    if (image == NULL)
    {
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }
    int mid = (image->width * image->height) / 2;

    //UPPER
    for (int i = 0; i < 20; i++)
        image->data[mid - 256 + (256 / 2) - 10 + i] = 255;
    //MID
    for (int i = 0; i < 20; i++)
        image->data[mid + (256 / 2) - 10 + i] = 255;
    //BOTTOM
    for (int i = 0; i < 20; i++)
        image->data[mid + 256 + (256 / 2) - 10 + i] = 255;

    vc_write_image("teste7.pbm", image);

    printf("Imagem criada: %d * %d\n", image->width, image->height);

    vc_image_free(image);

    return 0;
}