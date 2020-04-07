#include <stdio.h>
#include "vc.h"

int main()
{
    IVC *image;
    int x, y;
    long int pos;
    int line, col;

    image = vc_image_new(320, 320, 1, 1);

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
            printf("%ld\n", pos);

            line = x / (image->width / 8);
            col = y / (image->width / 8);

            if ((col + line) % 2 != 0)
            {
                image->data[pos] = 0;
            }
            else
            {
                image->data[pos] = 1;
            }
        }
    }

    vc_write_image("teste4.pbm", image);

    vc_image_free(image);

    printf("Press any key to exit...\n");
    getchar();
    return 0;
}