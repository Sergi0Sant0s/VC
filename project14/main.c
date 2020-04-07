#include <stdio.h>
#include <string.h>
#include "../librarys/vc.h"

char *conc(const char *first, char *second){
    char *aux = strdup(first);
    strcat(aux,second);
    return aux;
}

int main(void)
{
    const char *resourses = "../resourses/";
    const char *results = "../results/";
    
    IVC *image;
    IVC *rgb_result, *gray_result, *teste, *teste2;

    image = vc_read_image(conc(resourses,"Images/Classic/lenna.ppm"));
    gray_result = vc_image_new(image->width, image->height, 1, 255);
    rgb_result = vc_image_new(image->width, image->height, 3, 255);
    teste = vc_image_new(image->width, image->height, 1, 255);
    teste2 = vc_image_new(image->width, image->height, 1, 255);
    
    

    if (image == NULL)
    {
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }
    
    IVC *teste3 = vc_image_new(image->width, image->height, 1, 255);
    IVC *teste4 = vc_image_new(image->width, image->height, 1, 255);
    
    //vc_rgb_gray_to_binary_global_mean(image,teste3);
    //
    for (int y = 0; y < teste3->height; y++)
		for (int x = 0; x < teste3->width; x++)
		    teste3->data[y + x] = 0;
	//
    teste3->data[2] = 1;
    teste3->data[6] = 1;
    teste3->data[7] = 1;
    teste3->data[8] = 1;
    teste3->data[10] = 1;
    teste3->data[11] = 1;
    teste3->data[12] = 1;
    teste3->data[13] = 1;
    teste3->data[14] = 1;
    teste3->data[16] = 1;
    teste3->data[17] = 1;
    teste3->data[18] = 1;
    teste3->data[22] = 1;
    //vc_write_image(conc(results, "teste_prof.ppm"),teste3);
    
    //rgb_gray
    vc_rgb_gray(image,gray_result);
    
    // Gray_Erode_x
    vc_gray_erode_x(gray_result,teste4,5);
    vc_write_image(conc(results, "gray_erode_x.ppm"),teste4);
    
    // Gray_Erode_x
    vc_gray_dilate_x(gray_result,teste3,5);
    vc_write_image(conc(results, "gray_dilate_x.ppm"),teste3);
    
    
    //vc_gray_to_binary_midpoint(image, gray_result,33);
    //vc_rgb_gray_to_binary(image,gray_result,128);
    //vc_gray_negative(gray_result);
    //vc_rgb_gray(image,gray_result);
    
    //Bernsen
    //vc_gray_to_binary_bernsen(gray_result, teste, 25, 100);
    //vc_write_image(conc(results,"bernsen.ppm"), teste);
    
    // Niblack
    //vc_gray_to_binary_niblack(gray_result, teste, 15, -0.2);
    //vc_write_image(conc(results,"niblack.ppm"), teste);
    
    // Binary_mean + binary
    //vc_rgb_gray_to_binary_global_mean(gray_result,teste);
    //vc_gray_negative(gray_result);
    
    // Erode_x
    //vc_binary_erode_x(gray_result,teste4);
    //vc_write_image(conc(results, "erode_x.ppm"),teste4);
    
    // Erode
    //vc_binary_erode(teste3,teste2,5);
    //vc_write_image(conc(results, "erode.ppm"),teste2);
    
    // Dilate
    //vc_binary_dilate(gray_result,teste,3);
    //vc_write_image(conc(results, "dilate.ppm"),teste);
    
    // Open
    //vc_binary_open(gray_result,teste,5);
    //vc_write_image(conc(results, "close.ppm"),teste);
    
    // Close
    //vc_binary_close(gray_result,teste,5);
    //vc_write_image(conc(results, "close.ppm"),teste);
    
    vc_image_free(image);
    vc_image_free(gray_result);
    vc_image_free(rgb_result);
    vc_image_free(teste);
    vc_image_free(teste2);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}