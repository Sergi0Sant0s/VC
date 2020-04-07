//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Desabilita (no MSVC++) warnings de fun��es n�o seguras (fopen, sscanf, etc...)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "vc.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Alocar mem�ria para uma imagem
IVC *vc_image_new(int width, int height, int channels, int levels)
{
	IVC *image = (IVC *)malloc(sizeof(IVC));

	if (image == NULL)
		return NULL;
	if ((levels <= 0) || (levels > 255))
		return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	image->data = (unsigned char *)malloc(image->width * image->height * image->channels * sizeof(char));

	if (image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}

// Libertar mem�ria de uma imagem
IVC *vc_image_free(IVC *image)
{
	if (image != NULL)
	{
		if (image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

char *netpbm_get_token(FILE *file, char *tok, int len)
{
	char *t;
	int c;

	for (;;)
	{
		while (isspace(c = getc(file)))
			;
		if (c != '#')
			break;
		do
			c = getc(file);
		while ((c != '\n') && (c != EOF));
		if (c == EOF)
			break;
	}

	t = tok;

	if (c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while ((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));

		if (c == '#')
			ungetc(c, file);
	}

	*t = 0;

	return tok;
}

long int unsigned_char_to_bit(unsigned char *datauchar, unsigned char *databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char *p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}

void bit_to_unsigned_char(unsigned char *databit, unsigned char *datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char *p = databit;

	countbits = 1;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}

IVC *vc_read_image(char *filename)
{
	FILE *file = NULL;
	IVC *image = NULL;
	unsigned char *tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;

	// Abre o ficheiro
	if ((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if (strcmp(tok, "P4") == 0)
		{
			channels = 1;
			levels = 1;
		} // Se PBM (Binary [0,1])
		else if (strcmp(tok, "P5") == 0)
			channels = 1; // Se PGM (Gray [0,MAX(level,255)])
		else if (strcmp(tok, "P6") == 0)
			channels = 3; // Se PPM (RGB [0,MAX(level,255)])
		else
		{
#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
#endif

			fclose(file);
			return NULL;
		}

		if (levels == 1) // PBM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
					sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL)
				return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL)
				return 0;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			if ((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
					sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 ||
					sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL)
				return NULL;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			size = image->width * image->height * image->channels;

			if ((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}

		fclose(file);
	}
	else
	{
#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
#endif
	}

	return image;
}

int vc_write_image(char *filename, IVC *image)
{
	FILE *file = NULL;
	unsigned char *tmp;
	long int totalbytes, sizeofbinarydata;

	if (image == NULL)
		return 0;

	if ((file = fopen(filename, "wb")) != NULL)
	{
		if (image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char *)malloc(sizeofbinarydata);
			if (tmp == NULL)
				return 0;

			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);

			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if (fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);

			if (fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				return 0;
			}
		}

		fclose(file);

		return 1;
	}

	return 0;
}

/*
=======================================================================================
=======================================================================================
=======================================================================================
=======================================================================================
*/

int vc_rgb_gray(IVC *original, IVC *converted)
{
	int pos, posAux, x, y;
	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			posAux = y * converted->bytesperline + x * converted->channels;
			converted->data[posAux] = ((original->data[pos] + original->data[pos + 1] + original->data[pos + 2]) / 3);
		}
	}

	return 0;
}

int vc_rgb_negative(IVC *original, IVC *converted)
{
	int pos, x, y;
	int max = 255;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;

			converted->data[pos] = max - original->data[pos];
			converted->data[pos + 1] = max - original->data[pos + 1];
			converted->data[pos + 2] = max - original->data[pos + 2];
		}
	}

	return 0;
}

int vc_gray_negative(IVC *original)
{
	int pos, x, y;
	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			original->data[pos] = (255 - original->data[pos]);
		}
	}

	return 0;
}

int vc_only_red(IVC *original, IVC *converted)
{
	int pos, x, y;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			converted->data[pos] = original->data[pos];
			converted->data[pos + 1] = original->data[pos];
			converted->data[pos + 2] = original->data[pos];
		}
	}

	return 0;
}

int vc_only_green(IVC *original, IVC *converted)
{
	int pos, x, y;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			converted->data[pos + 1] = original->data[pos + 1];
			converted->data[pos] = 0;
			converted->data[pos + 2] = 0;
		}
	}

	return 0;
}

int vc_only_blue(IVC *original, IVC *converted)
{
	int pos, x, y;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			converted->data[pos + 2] = original->data[pos + 2];
			converted->data[pos + 1] = 0;
			converted->data[pos] = 0;
		}
	}

	return 0;
}

int vc_image_darker(IVC *original, IVC *converted, float perc)
{
	int pos, x, y;
	float totPerc = perc / 100;
	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			converted->data[pos + 2] = original->data[pos + 2] - (original->data[pos + 2] * totPerc);
			converted->data[pos + 1] = original->data[pos + 1] - (original->data[pos + 1] * totPerc);
			converted->data[pos] = original->data[pos] - (original->data[pos] * totPerc);
		}
	}

	return 0;
}

int vc_image_lighter(IVC *original, IVC *converted, float perc)
{
	int pos, x, y;
	float totPerc = perc / 100;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			converted->data[pos + 2] = original->data[pos + 2] + (original->data[pos + 2] * totPerc) < 255 ? original->data[pos + 2] + (original->data[pos + 2] * totPerc) : 255;
			converted->data[pos + 1] = original->data[pos + 1] + (original->data[pos + 1] * totPerc) < 255 ? original->data[pos + 1] + (original->data[pos + 1] * totPerc) : 255;
			converted->data[pos] = original->data[pos] + (original->data[pos] * totPerc) < 255 ? original->data[pos] + (original->data[pos] * totPerc) : 255;
		}
	}
	return 0;
}

int vc_remove_red(IVC *original, IVC *converted)
{
	int pos, x, y;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;

			converted->data[pos] = 0;
			converted->data[pos + 1] = original->data[pos + 1];
			converted->data[pos + 2] = original->data[pos + 2];
		}
	}
	return 0;
}

int vc_remove_green(IVC *original, IVC *converted)
{
	int pos, x, y;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			converted->data[pos] = original->data[pos];
			converted->data[pos + 1] = 0;
			converted->data[pos + 2] = original->data[pos + 2];
		}
	}
	return 0;
}

int vc_remove_blue(IVC *original, IVC *converted)
{
	int pos, x, y;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			converted->data[pos] = original->data[pos];
			converted->data[pos + 1] = original->data[pos + 1];
			converted->data[pos + 2] = 0;
		}
	}

	return 0;
}

int vc_change_channels(IVC *original, IVC *converted)
{
	int pos, x, y;
	int auxR, auxG, auxB;

	for (x = 0; x < original->width; x++)
	{
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			auxR = original->data[pos];
			auxG = original->data[pos + 1];
			auxB = original->data[pos + 2];
			converted->data[pos] = auxG;
			converted->data[pos + 1] = auxB;
			converted->data[pos + 2] = auxR;
		}
	}

	return 0;
}

//Calcular max e min para o hsv
float *min_max(float a, float b, float c){
	float *aux = malloc(sizeof(float) * 2);
	aux[0] =((a < b) ? (a < c ? a : c) : (b < c ? b : c));
	aux[1] = ((a > b) ? (a > c ? a : c) : (b > c ? b : c));
	return aux;
}

// Conversão de RGB para HSV
int vc_rgb_to_hsv(IVC *original, IVC *converted)
{
	float *aux = malloc(sizeof(float) * 3);
	float *aux2;
	
	unsigned char *data = (unsigned char *)converted->data;
	int width = original->width;
	int height = original->height;
	int bytesperline = original->bytesperline;
	int channels = original->channels;
	float r, g, b, hue, saturation, value;
	float rgb_max, rgb_min;
	int i, size;

	// Verificação de erros
	if ((width <= 0) || (height <= 0) || (original->data == NULL)) return 0;
	if (channels != 3) return 0;

	size = width * height * channels;

	for (i = 0; i<size; i = i + channels)
	{
		r = (float)original->data[i];
		g = (float)original->data[i + 1];
		b = (float)original->data[i + 2];

		// Calcula valores máximo e mínimo dos canais de cor R, G e B
		aux2 = min_max(r,g,b);
		rgb_min = aux2[0];
		rgb_max = aux2[1];

		// Value toma valores entre [0,255]
		value = rgb_max;
		if (value == 0.0f)
		{
			hue = 0.0f;
			saturation = 0.0f;
		}
		else
		{
			// Saturation toma valores entre [0,255]
			saturation = ((rgb_max - rgb_min) / rgb_max) * 255.0f;

			if (saturation == 0.0f)
				hue = 0.0f;
			else
			{
				// Hue toma valores entre [0,360]
				if ((rgb_max == r) && (g >= b))
					hue = 60.0f * (g - b) / (rgb_max - rgb_min);
				else if ((rgb_max == r) && (b > g))
					hue = 360.0f + 60.0f * (g - b) / (rgb_max - rgb_min);
				else if (rgb_max == g)
					hue = 120.0f + 60.0f * (b - r) / (rgb_max - rgb_min);
				else /* rgb_max == b*/
					hue = 240.0f + 60.0f * (r - g) / (rgb_max - rgb_min);
			}
		}

		// Atribui valores entre [0,255]
		data[i] = (unsigned char) (hue / 360.0f * 255.0f);
		data[i + 1] = (unsigned char) (saturation);
		data[i + 2] = (unsigned char) (value);
	}

	return 1;
}


// hmin,hmax = [0, 360]; smin,smax = [0, 100]; vmin,vmax = [0, 100]
int vc_hsv_segmentation(IVC *original,IVC *converted, int hmin, int hmax, int smin, int smax, int vmin, int vmax)
{
	unsigned char *data = (unsigned char *)converted->data;
	int width = original->width;
	int height = original->height;
	int bytesperline = original->bytesperline;
	int channels = original->channels;
	int hue, saturation, value;
	int h, s, v; // h=[0, 360] s=[0, 100] v=[0, 100]
	int i, size;

	// Verificação de erros
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if (channels != 3) return 0;

	size = width * height * channels;

	for (i = 0; i < size; i = i + channels)
	{
		h = ((float)original->data[i]) / 255.0f * 360.0f;
		s = ((float)original->data[i + 1]) / 255.0f * 100.0f;
		v = ((float)original->data[i + 2]) / 255.0f * 100.0f;

		if ((h > hmin) && (h <= hmax) && (s >= smin) && (s <= smax) && (v >= vmin) && (v <= vmax))
		{
			data[i] = 255;
			data[i + 1] = 255;
			data[i + 2] = 255;
		}
		else
		{
			data[i] = 0;
			data[i + 1] = 0;
			data[i + 2] = 0;
		}
	}

	return 1;
}

int vc_scale_gray_to_rgb(IVC *original, IVC *converted){
	int pos, posAux, x, y;
	float value;
	unsigned char *_original = (unsigned char *)original->data;
	unsigned char *_converted = (unsigned char *)converted->data;
	for (x = 0; x < original->width; x++){
		for (y = 0; y < original->height; y++){
			pos = y * original->bytesperline + x * original->channels;
			posAux = y * converted->bytesperline + x * converted->channels;
			
			if(_original[pos] < 64){
				_converted[posAux] = (unsigned char)0;
				_converted[posAux + 1] = (unsigned char)(4.047619048 * _original[pos]);
				_converted[posAux + 2] = (unsigned char)255;
				if(_original[pos] == 0) 
					printf("G: %f\tC: %d\n", 4.047619048 * _original[pos], _converted[posAux+1]);
			}
			else if(_original[pos] >= 64 && _original[pos] < 128){
				_converted[posAux] = 0;
				_converted[posAux + 1] = 255;
				_converted[posAux + 2] = (int)(((129 - _original[pos]) * 255)/64);
			}
			else if(_original[pos] >= 128 && _original[pos] < 192){
				_converted[posAux] = ceil(((_original[pos]-128.0)/63.0) * 255);
				_converted[posAux + 1] = 255;
				_converted[posAux + 2] = 0;
			}
			else{
				_converted[posAux] = 255;
				_converted[posAux + 1] = (int)(((255 - _original[pos]) * 255)/64);
				_converted[posAux + 2] = 0;
			}
		}
	}
	return 1;
}

float vc_media(IVC *original){
	int pos,x,y;
	float media = 0;
	
	
	if(original->channels == 1){
		for (x = 0; x < original->width; x++)
			for (y = 0; y < original->height; y++)
			{
				pos = y * original->bytesperline + x * original->channels;
				media += original->data[pos];
			}
	}
	else if(original->channels == 3)
	{
		for (x = 0; x < original->width; x++)
			for (y = 0; y < original->height; y++)
			{
				pos = y * original->bytesperline + x * original->channels;
				media += (original->data[pos] + original->data[pos + 1] + original->data[pos + 2])/3;
			}
	}
	return (media/(original->width * original->height));
}

int vc_rgb_gray_to_binary_global_mean(IVC *original, IVC *converted){
	int pos,posAux,x,y;
	float media = vc_media(original), meanAux = 0;
	
	
	for (x = 0; x < original->width; x++){
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			posAux = y * converted->bytesperline + x * converted->channels;
			meanAux = (original->channels == 1 ? original->data[pos] : (original->data[pos] + original->data[pos+1] + original->data[pos+2]))/original->channels; 
			converted->data[posAux] = meanAux > media ? 255 : 0;
		}
	}
	
	return 1;
}

int vc_rgb_gray_to_binary(IVC *original, IVC *converted, int threshold){
	int pos,posAux,x,y;
	float meanAux;
	
	for (x = 0; x < original->width; x++){
		for (y = 0; y < original->height; y++)
		{
			pos = y * original->bytesperline + x * original->channels;
			posAux = y * converted->bytesperline + x * converted->channels;
			meanAux = (original->channels == 1 ? original->data[pos] : (original->data[pos] + original->data[pos+1] + original->data[pos+2]))/original->channels; 
			converted->data[posAux] = meanAux > threshold ? 255 : 0;
		}
	}
	
	return 1;
}