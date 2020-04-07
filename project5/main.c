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

    for (x = 0; x < image->width; x++)
    {
        for (y = 0; y < image->height; y++)
        {
            pos = y * image->bytesperline + x * image->channels;
            if (pos > mid)
            {
                image->data[pos] = (image->width * (((image->width * image->height) - pos))) / mid; //(x-y)/2;
            }
            else
            {
                image->data[pos] = (image->width * pos) / mid; //(x+y)/2;
            }
        }
    }

    vc_write_image("teste4.pbm", image);

    printf("Imagem criada: %d * %d\n", image->width, image->height);

    vc_image_free(image);

    return 0;
}