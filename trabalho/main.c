#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <strings.h>
#include "../librarys/vc.h"


int main(void)
{
    const char *resourses = "../resourses/";
    
    IVC *image;
    IVC *rgb_result, *gray_result, *gray_result1, *gray_result2, *teste, *teste1, *rgb_negative, *result_histagram;

    image = vc_read_image(conc(resourses,"img_seg/coins.pgm"));
    gray_result = vc_image_new(image->width, image->height, 1, 255);
    gray_result1 = vc_image_new(image->width, image->height, 1, 255);
    gray_result2 = vc_image_new(image->width, image->height, 1, 255);
    rgb_result = vc_image_new(image->width, image->height, 3, 255);
    rgb_negative = vc_image_new(image->width, image->height, 3, 255);
    teste1 = vc_image_new(255 * 3, 330, 3, 255);
    
    IVC *image_label = vc_image_new(5,5,1,255);
    IVC *binnary = vc_image_new(image->width, image->height, 1, 255);
    
    if (image == NULL)
    {
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }
    
    vc_rgb_gray(image,gray_result);
    vc_rgb_gray_to_binary_global_mean(gray_result,gray_result1);
    save_run("teste1.pgm", gray_result);
    int nAux = 0;
    OVC *aux;
    aux = vc_binary_blob_labelling(gray_result1, gray_result2, &nAux);
    
    //printf("FECHOU\n");
    //printf("Valor: %d", *nAux);
    //getchar();
    
    save_run("teste.pgm", teste);
    
    vc_image_free(image);
    vc_image_free(gray_result);
    vc_image_free(rgb_result);

    return 0;
}