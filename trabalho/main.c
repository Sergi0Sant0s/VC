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
    IVC *rgb_result,*rgb_result1, *gray_result, *gray_result1, *gray_result2,*gray_result3, *teste, *teste1, *rgb_negative, *result_histagram;

    image = vc_read_image(conc(resourses,"trab/img3.ppm"));
    gray_result = vc_image_new(image->width, image->height, 1, 255);
    gray_result1 = vc_image_new(image->width, image->height, 1, 255);
    gray_result2 = vc_image_new(image->width, image->height, 1, 255);
    gray_result3 = vc_image_new(image->width, image->height, 1, 255);
    rgb_result = vc_image_new(image->width, image->height, 3, 255);
    rgb_result1 = vc_image_new(image->width, image->height, 3, 255);
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
    
    //c_image_darker(image,rgb_result,80);
    /*save_run("darker.pgm", image);
    vc_rgb_gray(image,gray_result);
    save_run("rgb_gray.pgm", gray_result);
    
    vc_rgb_gray_to_binary_global_mean(gray_result,gray_result1);
    save_run("binnary.pgm", gray_result1);
    vc_binary_erode_x(gray_result1,gray_result2,3);
    save_run("erode.pgm", gray_result2);*/
    
    vc_rgb_histogram_equalization(image,rgb_result);
    vc_teste(rgb_result, rgb_result1);
    vc_rgb_gray_to_binary(rgb_result1,gray_result, 40);
    //vc_binary_erode(gray_result,gray_result1,5);
    //save_run("teste2.pbm", gray_result);
    //save_run("teste3.pbm", gray_result1);
    //save_run("teste.pgm", rgb_result);
    
    printf("Chega\n");
    int nBlobs = 0;
    OVC *blobs = vc_binary_blob_labelling(gray_result, gray_result2, &nBlobs);
    printf("Passa\n");
    vc_binary_blob_info(gray_result2,blobs,nBlobs);
    printf("Sai\n");
    for(int i = 0;i < nBlobs;i++){
        printf("Label: %d | Perimetro: %d | Centro-MASSA: %d,%d | Area: %d | Width: %d | Height: %d\n",blobs[i].label,blobs[i].perimeter,blobs[i].xc,blobs[i].yc,blobs[i].area,blobs[i].width,blobs[i].height);
    }
    
    
    //printf("\nnumero blobs: %d\n", nBlobs);
    //getchar();
    
    //save_run("teste.pgm", gray_result2);
    
    vc_image_free(image);
    vc_image_free(gray_result);
    vc_image_free(rgb_result);

    return 0;
}