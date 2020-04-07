#include <stdio.h>
#include <stdlib.h>
#include "vc.h"

int vc_gray_negative(IVC *imagem)
{
    int column;
    for (column = 0; column < image->width / 2; column++)

    {

        destPointer = data + lineSize * line + (column)*image->channels;

        sourcePointer = data + lineSize * line + (image->width - column) * image->channels;

        SwapChar(destPointer, sourcePointer, image->channels);
    }
}

void SwapChar(unsigned char *A, unsigned char *B, int number)

{

    unsigned char *C;

    C = (unsigned char *)calloc(number, sizeof(unsigned char));

    memcpy(C, A, sizeof(unsigned char) * number);

    memcpy(A, B, sizeof(unsigned char) * number);

    memcpy(B, C, sizeof(unsigned char) * number);

    free(C);
}

int main()
{
    IVC *image;
    int i;

    image = vc_read_image("teste.pbm");
    if (image == NULL)
    {
        printf("Error -> vc_read_image():\n\tFile not found!\n");
        getchar();
        return 0;
    }

    for (i = 0; i < image->bytesperline * image->height; i += image->channels)
    {
        if (image->data[i] == 1)
            image->data[i] = 0;
        else
            image->data[i] = 1;
    }

    vc_write_image("teste2.pbm", image);

    vc_image_free(image);

    printf("Press any key to exit...\n");
    getchar();
    return 0;
}