#include <stdio.h>
#include "vc.h"

int main(){

    IVC *image;
    int x,y;
    long int pos;

    image = vc_image_new(256,256,1,255);

    if(image == NULL){
        printf("Error -> vc_image_new():\n\tOut of memory!\n");
        getchar();
        return 0;
    }
    int mid = (image->width * image->height)/2;
    int contador = 0;
    
    
    for(x = 0;x < image->width; x++){
        for(y = 0; y < (image->height - contador); y++){
            pos = y * image->bytesperline + x * image->channels;
            image->data[pos] = (x+y)/2;
        }
        contador++;
    }

    contador = 0;
    for(x = 256;x > 0; x--){
        for(y = 0 + contador; y < (image->height); y++){
            pos = y * image->bytesperline + x * image->channels;
            image->data[pos] = 256 - (x+y)/2;
        }
        contador++;
    }
    

    vc_write_image("teste5.pbm", image);
    
    printf("Imagem criada: %d * %d\n",image->width,image->height);
    
    vc_image_free(image);

    return 0;
}