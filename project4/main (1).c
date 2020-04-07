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

    for (x = 0; x < image->width; x++)
    {
        for (y = 0; y < image->height; y++)
        {
            pos = y * image->bytesperline + x * image->channels;
            image->data[pos] = (x + y) / 2;
            printf("pos: %ld | bytesperline: %d | channels: %d\n", pos, image->bytesperline, image->channels);
        }
    }

    vc_write_image("teste3.pbm", image);

    vc_image_free(image);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}