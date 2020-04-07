#include <stdio.h>
#include <string.h>
#include "../librarys/vc.h"

char *conc(const char *first, char *second){
    char *aux = strdup(first);
    strcat(aux,second);
    return aux;
}

int vc_rgb_to_gray_2(IVC *src, IVC *dst)
{
	unsigned char *datasrc = (unsigned char *)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char *datadst = (unsigned char *)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float rf, gf, bf;

	//verificação de errros 
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height)) return 0;
	if ((src->channels != 3) || (dst->channels != 1)) return 0;

	for (y = 0; y <= height; y++)
	{
		for (x = 0; x <= width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;
			rf = (float)datasrc[pos_src];
			gf = (float)datasrc[pos_src + 1];
			bf = (float)datasrc[pos_src + 2];

			datadst[pos_dst] = (unsigned char)((rf * 0.299) + (gf * 0.587) + (bf * 0.114));
		}
	}
	return 1;
}

int main(void)
{
    const char *resourses = "../resourses/";
    const char *results = "../results/";
    
    IVC *image;
    IVC *rgb_result, *gray_result, *teste, *teste2,*teste3,*teste4,*teste5,*rgb_negative;

    image = vc_read_image(conc(resourses,"Imagens/HSVTestImage01.ppm"));
    gray_result = vc_image_new(image->width, image->height, 1, 255);
    teste = vc_image_new(image->width, image->height, 1, 255);
    teste2 = vc_image_new(image->width, image->height, 1, 255);
    teste3 = vc_image_new(image->width, image->height, 1, 255);
    teste4 = vc_image_new(image->width, image->height, 1, 255);
    teste5 = vc_image_new(image->width, image->height, 1, 255);
    rgb_result = vc_image_new(image->width, image->height, 3, 255);
    rgb_negative = vc_image_new(image->width, image->height, 3, 255);
    
    IVC *image_label = vc_image_new(5,5,1,255);
    IVC *binnary = vc_image_new(image->width, image->height, 1, 255);
    
    for(int i = 0;i< 25;i++)
        image_label->data[i] = 255;
        
    image_label->data[1] = 0;
    image_label->data[6] = 0;
    image_label->data[11] = 0;
    image_label->data[13] = 0;
    image_label->data[18] = 0;
    image_label->data[23] = 0;
    
    if (image == NULL)
    {
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }
    
    vc_rgb_gray_to_binary_global_mean(image,gray_result);
    //vc_binary_erode(gray_result,teste,5);
    //vc_binary_dilate(gray_result,teste2,15);
    //vc_binary_erode(teste2,gray_result,17);
    vc_write_image(conc(results,"teste_binnary.ppm"),gray_result);
    vc_binary_blob_labelling(gray_result, binnary);
    vc_write_image(conc(results,"teste_label.ppm"),binnary);
    
    //vc_rgb_to_gray_2(image, gray_result);
    //vc_write_image(conc(results,"rgb_negative.ppm"),gray_result);
    
    /*vc_rgb_negative(image,rgb_negative);
    vc_write_image(conc(results,"rgb_negative.ppm"),rgb_negative);
    //
    vc_image_lighter(rgb_negative,rgb_result,40);
    vc_write_image(conc(results,"rgb_lighter.ppm"),rgb_result);
    //
    vc_rgb_gray(rgb_result,gray_result);
    vc_write_image(conc(results,"rgb_gray.ppm"),gray_result);
    //
    vc_gray_to_binary_bernsen(gray_result,teste,5,25);
    vc_write_image(conc(results,"binnary_mean_2.ppm"),teste);
    
    for(int i = 0;i<20;i++){
        //vc_binary_dilate(teste,teste2,5);
        //vc_binary_erode(teste2,teste3,5);
        vc_binary_open(teste,teste2,3);
        vc_binary_close(teste2,teste3,3);
    }
    vc_write_image(conc(results,"final.ppm"),teste3);
    
    
    */
    
    //
    //vc_write_image(conc(results,"original.ppm"),image);
    //
    //vc_rgb_gray_to_binary_global_mean(image,teste);
    //vc_gray_to_binary_niblack(image, teste, 3, -0.2);
    //vc_write_image(conc(results,"binnary_mean_2.ppm"),teste);
    //
    //vc_binary_dilate(teste,teste2,3);
    //vc_binary_blob_labelling(teste,teste2);
    //vc_write_image(conc(results,"dilate.ppm"),teste2);
    
    
    
    vc_image_free(image);
    vc_image_free(gray_result);
    vc_image_free(rgb_result);

    printf("Press any key to exit...\n");
    getchar();

    return 0;
}