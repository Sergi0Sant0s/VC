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

#pragma region Includes

//Includes do sistema
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
// Inlcudes
#include "vc.h"

#pragma endregion

#pragma region Variables
//Pasta onde serão guardados os resultados obtidos
const char *results = "../results/";

#pragma endregion

#pragma region Funcoes ALOCAR E LIBERTAR UMA IMAGEM

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
			printf("============================== IMAGEM ABERTA ==================================\n");
			printf("Imagem: %d x %d | Channels: %d | Levels: %d\n", image->width, image->height, image->channels, image->levels);
			printf("Localização: %s", filename);
			printf("\n===============================================================================\n\n");
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
			printf("============================== IMAGEM ABERTA ==================================\n");
			printf("Imagem: %d x %d | Channels: %d | Levels: %d\n", image->width, image->height, image->channels, image->levels);
			printf("Localização: %s", filename);
			printf("\n===============================================================================\n\n");
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
		printf("============================== IMAGEM GERADA ==================================\n");
		printf("Imagem: %d x %d | Channels: %d | Levels: %d\n", image->width, image->height, image->channels, image->levels);
		printf("Localização: %s\n",filename);
		printf("===============================================================================\n\n");
		fclose(file);

		return 1;
	}

	return 0;
}

#pragma endregion

#pragma region Save and Run

char *conc(const char *first, char *second){
    char *aux = strdup(first);
    strcat(aux,second);
    return aux;
}

void execute(char *filepath){
    int status;
    
    char *array[5];
    //
    array[0] = "open";
    array[1] = "-a";
    array[2] = "Preview";
    array[3] = filepath;
    array[4] = NULL;
    //
    execvp(array[0], array);
}

void save(char *filename, IVC * image){
    char *filepath = conc(results,filename);
    vc_write_image(filepath,image);
}

void save_run(char *filename, IVC * image){
	char *filepath = strdup(results);
    strcat(filepath,filename);
    vc_write_image(filepath,image);
    execute(filepath); 
}

#pragma endregion

#pragma region RGB to GRAY

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

#pragma endregion

#pragma region Negativos

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

#pragma endregion

#pragma region Luminosidade

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

#pragma endregion

#pragma region Manuseamento de channels

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

#pragma endregion

#pragma region RGB to HSV

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

#pragma endregion

#pragma region Segmentacao

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

#pragma endregion

#pragma region exercicio scale aula

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

#pragma endregion

#pragma region Binary

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
			converted->data[posAux] = meanAux > threshold ? 0 : 255;
		}
	}
	
	return 1;
}

int vc_gray_to_binary_midpoint(IVC *original, IVC *converted, int kernel){
	long int pos, posk;
	float meanAux;
	int channels = original->channels, bytesperline = original->bytesperline;
	int offset = (kernel - 1)/2;
	int x,kx,y,ky;
	int width = original->width, height = original->height;
	int max = 0, min = 255;
	float threshold;
	
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return 0;
	if (channels != 1) return 0;
	
	
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{
			pos = y * original->bytesperline + x * original->channels;
			min = 255;
			max = 0;
			//
			for(ky = -offset; ky <= offset; ky++)
				for(kx = -offset; kx <= offset; kx++)
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
					{
						posk = (y + ky) * bytesperline + (x + kx) * channels;   
						
						if(original->data[posk] > max) max = original->data[posk];
						if(original->data[posk] < min) min = original->data[posk];
					}
			//
			threshold = (unsigned char)((float)(max + min) / (float)2);
			
			if(original->data[pos] > threshold) converted->data[pos] = 0;
			else converted->data[pos] = 255;
		}
		
	return 1;
}

int vc_gray_to_binary_bernsen(IVC *original, IVC *converted, int kernel, int c){
	long int pos, posk;
	float meanAux;
	int channels = original->channels, bytesperline = original->bytesperline;
	int offset = (kernel - 1)/2;
	int x,kx,y,ky;
	int width = original->width, height = original->height;
	int max = 0, min = 255;
	float threshold;
	int count = 0;
	
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return 0;
	if (channels != 1) return 0;
	
	
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{
			pos = y * original->bytesperline + x * original->channels;
			min = 255;
			max = 0;
			count = 0;
			//
			for(ky = -offset; ky <= offset; ky++)
				for(kx = -offset; kx <= offset; kx++){
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
						{
							posk = (y + ky) * bytesperline + (x + kx) * channels;   
							
							if(original->data[posk] > max) max = original->data[posk];
							if(original->data[posk] < min) min = original->data[posk];
						}
				}
				
			//
			if((max - min) < c)
				threshold = 255 / 2;
			else
				threshold = (unsigned char)((float)(max + min) / (float)2);
			
			if(original->data[pos] > threshold) converted->data[pos] = 0;
			else converted->data[pos] = 255;
		}
		
	return 1;
}

int vc_gray_to_binary_niblack(IVC *original, IVC *converted, int kernel, float k){
	long int pos, posk;
	float meanAux;
	int channels = original->channels, bytesperline = original->bytesperline;
	int offset = (kernel - 1)/2;
	int x,kx,y,ky;
	int width = original->width, height = original->height;
	float mean, dv;
	int count;
	float threshold;
	
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return 0;
	if (channels != 1) return 0;
	
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{
			pos = y * original->bytesperline + x * original->channels;
			mean = 0;
			dv = 0;
			count = 0;
			//
			for(ky = -offset; ky <= offset; ky++)
				for(kx = -offset; kx <= offset; kx++){
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
						{
							posk = (y + ky) * bytesperline + (x + kx) * channels;   
							//
							mean += original->data[posk];
							count++;
						}
				}
			
			//Mean
			mean /= count;
				
			for(ky = -offset; ky <= offset; ky++)
				for(kx = -offset; kx <= offset; kx++){
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
						{
							posk = (y + ky) * bytesperline + (x + kx) * channels;   
							//
							dv += pow((original->data[posk] - mean),2);
						}
				}
				
			//calculo final
			threshold = mean + k *  sqrt(dv / count);
			
			//aplica o thresholding
			if(original->data[pos] > threshold) converted->data[pos] = 0;
			else converted->data[pos] = 255;
		}
		
	return 1;
}

int vc_binary_dilate(IVC *original, IVC *converted, int kernel){
	long int pos, posk;
	float meanAux;
	int channels = original->channels, bytesperline = original->bytesperline;
	int offset = (kernel - 1)/2;
	int x,kx,y,ky;
	int width = original->width, height = original->height;
	float threshold;
	
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return 0;
	if (channels != 1) return 0;
	
	
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{
			pos = y * original->bytesperline + x * original->channels;
			//
			converted->data[pos] = 0;
			for(ky = -offset; ky <= offset; ky++)
				for(kx = -offset; kx <= offset; kx++)
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
						{
							posk = (y + ky) * bytesperline + (x + kx) * channels;   
							//
							if(original->data[posk] == 255) 
								converted->data[pos] = 255;
						}
		}
		
	return 1;
}

int vc_binary_erode(IVC *original, IVC *converted, int kernel){
	long int pos, posk;
	float meanAux;
	int channels = original->channels, bytesperline = original->bytesperline;
	int offset = (kernel - 1)/2;
	int x,kx,y,ky;
	int width = original->width, height = original->height;
	float threshold;
	
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return 0;
	if (channels != 1) return 0;
	
	
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{
			pos = y * original->bytesperline + x * original->channels;
			//
			converted->data[pos] = 255;
			for(ky = -offset; ky <= offset; ky++)
				for(kx = -offset; kx <= offset; kx++)
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
						{
							posk = (y + ky) * bytesperline + (x + kx) * channels;   
							//
							if(original->data[posk] == 0) 
								converted->data[pos] = 0;
						}
		}
		
	return 1;
}

int vc_gray_dilate_x(IVC *original, IVC *converted, int kernel){
	long int pos, posk;
	float meanAux;
	int channels = original->channels, bytesperline = original->bytesperline;
	int offset = (kernel - 1)/2 , offset_dyn;
	int x,kx,y,ky;
	int width = original->width, height = original->height;
	float threshold;
	int auxCol, auxLine;
	int max = 0;
	
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return 0;
	if (channels != 1) return 0;
	
	
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{   
			offset_dyn = 0;
			pos = y * original->bytesperline + x * original->channels;
			max = 0;
			//
			converted->data[pos] = 255;
			for(ky = -offset; ky <= offset; ky++){
				for(kx = -offset_dyn; kx <= offset_dyn; kx++){
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
					{
						posk = (y + ky) * bytesperline + (x + kx) * channels;   
						//
						if(original->data[posk] > max) 
							max = original->data[posk];
					}
				}
				if(ky < 0) offset_dyn++;
				else offset_dyn--;
			}
			converted->data[pos] = max;
		}
		
	return 1;
}

int vc_gray_erode_x(IVC *original, IVC *converted, int kernel){
	long int pos, posk;
	float meanAux;
	int channels = original->channels, bytesperline = original->bytesperline;
	int offset = (kernel - 1)/2 , offset_dyn;
	int x,kx,y,ky;
	int width = original->width, height = original->height;
	float threshold;
	int auxCol, auxLine;
	int min = 255;
	
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return 0;
	if (channels != 1) return 0;
	
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{   
			offset_dyn = 0;
			pos = y * original->bytesperline + x * original->channels;
			min = 255;
			//
			for(ky = -offset; ky <= offset; ky++){
				for(kx = -offset_dyn; kx <= offset_dyn; kx++){
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
					{
						posk = (y + ky) * bytesperline + (x + kx) * channels;   
						//
						if(original->data[posk] < min) 
							min = original->data[posk];
					}
				}
				if(ky < 0) offset_dyn++;
				else offset_dyn--;
			}
			converted->data[pos] = min;
		}
		
	return 1;
}

int vc_binary_open(IVC *original, IVC *converted, int kernel){
	IVC * temp;
	temp = vc_image_new(original->width, original->height, 1, 255);
	
	//Erode
	vc_binary_erode(original,temp,kernel);
	//Dilate
	vc_binary_dilate(temp,converted,kernel);
	
	return 1;
}

int vc_binary_close(IVC *original, IVC *converted, int kernel){
	IVC * temp;
	temp = vc_image_new(original->width, original->height, 1, 255);
	
	//Dilate
	vc_binary_dilate(original,temp,kernel);
	//Erode
	vc_binary_erode(temp,converted,kernel);
	
	return 1;
}

int vc_binary_erode_x(IVC *original, IVC *converted, int kernel){

	//VARS
	long int pos, posk;
	float meanAux;
	int channels = original->channels, bytesperline = original->bytesperline;
	int offset = (kernel - 1)/2 , offset_dyn;
	int x,kx,y,ky;
	int width = original->width, height = original->height;
	float threshold;
	int auxCol, auxLine;
	
	//CODE
	
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return 0;
	if (channels != 1) return 0;
	
	
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{   
			offset_dyn = 0;
			pos = y * original->bytesperline + x * original->channels;
			//
			converted->data[pos] = 255;
			for(ky = -offset; ky <= offset; ky++){
				for(kx = -offset_dyn; kx <= offset_dyn; kx++){
					if((y + ky >= 0) && (y + ky < height) && (x + kx >= 0) && (x + kx < width))
					{
						posk = (y + ky) * bytesperline + (x + kx) * channels;   
						//
						if(original->data[posk] == 0) 
							converted->data[pos] = 0;
					}
				}
				if(ky < 0) offset_dyn++;
				else offset_dyn--;
			}
		}
		
	return 1;
}

#pragma endregion

#pragma region Labeling
// Etiquetagem de blobs
// src		: Imagem binaria de entrada
// dst		: Imagem grayscale (ira conter as etiquetas)
// nlabels	: Endereco de memoria de uma varievel, onde sera armazenado o numero de etiquetas encontradas.
// OVC*		: Retorna um array de estruturas de blobs (objectos), com respectivas etiquetas. � necessario libertar posteriormente esta memoria.
OVC* vc_binary_blob_labelling(IVC *original, IVC *converted, int *nlabels)
{
	unsigned char *datasrc = (unsigned char *)original->data;
	unsigned char *datadst = (unsigned char *)converted->data;
	int width = original->width;
	int height = original->height;
	int bytesperline = original->bytesperline;
	int channels = original->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = { 0 }; //Primeiro elemento do array = 0
	int labelarea[256] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num, tmplabel;
	OVC *blobs; // Apontador para array de blobs (objectos) que ser� retornado desta fun��o.

	// Verificao de erros
	if ((original->width <= 0) || (original->height <= 0) || (original->data == NULL)) return 0;
	if ((original->width != converted->width) || (original->height != converted->height) || (original->channels != converted->channels)) return NULL;
	if (channels != 1) return NULL;

	// Copia dados da imagem binaria para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pixeis de plano de fundo devem obrigatoriamente ter valor 0
	// Todos os pixeis de primeiro plano devem obrigatoriamente ter valor 255
	// Seria atribuidas etiquetas no intervalo [1,254]
	// Este algoritmo esta assim limitado a 255 labels
	//Default do array de destino
	for (i = 0, size = bytesperline * height; i<size; i++)
		if (datadst[i] != 0) 
			datadst[i] = 255;

	// Limpa os rebordos da imagem binaria (deixa de ser necessário verificar se o kernel esta dentro da imagem)
	for (y = 0; y<height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x<width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y<height - 1; y++)
		for (x = 1; x<width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X
			
			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels; // B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels; // D
			posX = y * bytesperline + x * channels; // X

													// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					//getchar();
					// Se A esta marcado
					if (datadst[posA] != 0) num = labeltable[datadst[posA]];
					// Se B esta marcado, e o menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (labeltable[datadst[posB]] < num)) num = labeltable[datadst[posB]];
					// Se C esta marcado, e o menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (labeltable[datadst[posC]] < num)) num = labeltable[datadst[posC]];
					// Se D esta marcado, e o menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (labeltable[datadst[posD]] < num)) num = labeltable[datadst[posD]];

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
					labeltable[num] = num;

					// Actualiza a tabela de etiquetas
					if (datadst[posA] != 0)
						if (labeltable[datadst[posA]] != num)
							for (tmplabel = labeltable[datadst[posA]], a = 1; a<label; a++)
								if (labeltable[a] == tmplabel)
									labeltable[a] = num;
									
					if (datadst[posB] != 0)
						if (labeltable[datadst[posB]] != num)
							for (tmplabel = labeltable[datadst[posB]], a = 1; a<label; a++)
								if (labeltable[a] == tmplabel)
									labeltable[a] = num;
									
					if (datadst[posC] != 0)
						if (labeltable[datadst[posC]] != num)
							for (tmplabel = labeltable[datadst[posC]], a = 1; a<label; a++)
								if (labeltable[a] == tmplabel)
									labeltable[a] = num;
									
					if (datadst[posD] != 0)
						if (labeltable[datadst[posD]] != num)
							for (tmplabel = labeltable[datadst[posC]], a = 1; a<label; a++)
								if (labeltable[a] == tmplabel)
									labeltable[a] = num;
				}
			}
		}
		

	// Volta a etiquetar a imagem
	for (y = 1; y<height - 1; y++)
		for (x = 1; x<width - 1; x++){
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
				datadst[posX] = labeltable[datadst[posX]];
			}

	// Contagem do numero de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a<label - 1; a++)
		for (b = a + 1; b<label; b++)
		{
			if (labeltable[a] == labeltable[b]) labeltable[b] = 0;
		}

	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que n�o hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a<label; a++)
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++; // Conta etiquetas
		}
	
	// Se nao ha blobs
	if (*nlabels == 0) return NULL;
	
	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC *)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
		for (a = 0; a<(*nlabels); a++) 
			blobs[a].label = labeltable[a];
	else 
		return NULL;

	return blobs;
}


int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs)
{
	unsigned char *data = (unsigned char *)src->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, i;
	long int pos;
	int xmin, ymin, xmax, ymax;
	long int sumx, sumy;

	// Verifica��o de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1) return 0;

	// Conta �rea de cada blob
	for (i = 0; i<nblobs; i++)
	{
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		blobs[i].area = 0;

		for (y = 1; y<height - 1; y++)
			for (x = 1; x<width - 1; x++)
			{
				pos = y * bytesperline + x * channels;

				if (data[pos] == blobs[i].label)
				{
					// �rea
					blobs[i].area++;

					// Centro de Gravidade
					sumx += x;
					sumy += y;

					// Bounding Box
					if (xmin > x) xmin = x;
					if (ymin > y) ymin = y;
					if (xmax < x) xmax = x;
					if (ymax < y) ymax = y;

					// Per�metro
					// Se pelo menos um dos quatro vizinhos n�o pertence ao mesmo label, ent�o � um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label))
						blobs[i].perimeter++;
				}
			}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		//blobs[i].xc = (xmax - xmin) / 2;
		//blobs[i].yc = (ymax - ymin) / 2;
		blobs[i].xc = sumx / MAX(blobs[i].area, 1);
		blobs[i].yc = sumy / MAX(blobs[i].area, 1);
	}

	return 1;
}

#pragma endregion

#pragma region Histograma

float *vc_histogram_array(IVC *original){
	//VARS
	int pos, y , x, max = original->levels + 1, maior = 0;
	float *array;
	
	//Inicializa array
	array = malloc(max);
	array[0] = 0;
	for(int i = 1; i < max; i++) array[i] = 0;
	
	//Gera vetor do histogram
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{   
			pos = y * original->bytesperline + x * original->channels;
			if(original->data[pos] != 0)
				array[original->data[pos]]++;
		}
	
	return array;
}

int vc_gray_histogram_show(IVC *original, IVC *converted){
	//VARS
	int pos, y , x, max = original->levels + 1, maior = 0;
	float *array;
	int n = original->width * original->height;
	
	array = vc_histogram_array(original);
	
	//Calcular index com maior valor
	for(int i = 1; i < max; i++){
		if(array[i] > maior) maior = array[i];
	}
	
	//coloca os valores entre 0-300
	for(int i = 1; i < max; i++){
		array[i] = (array[i] * 300) / maior;
	}
	
	//Paint
	for (y = 0; y < converted->height; y++)
		for (x = 0; x < converted->width; x++)
		{   
			pos = y * converted->bytesperline + x * converted->channels;
			//Reset
			converted->data[pos] = 0;
			
			//Paint
			if(y > 20 && y <= 320 && x <= 255){
				if((320 - y) > array[x]){
					converted->data[pos] = 255;
				}
			}
			else if(y > 320)
				converted->data[pos] = x;
			else
				converted->data[pos] = 255;
		}
	return 1;
}

int vc_rgb_histogram_show(IVC *original, IVC *converted){
	//VARS
	int pos, y , x, max = original->levels + 1, i;
	float r[max],g[max],b[max];
	int maior_r,maior_g,maior_b;
	int n = original->width * original->height;
	
	//Inicializa array
	r[0] = 0;
	g[0] = 0;
	b[0] = 0;
	for(i = 1; i < max; i++) {
		r[i] = 0;
		g[i] = 0;
		b[i] = 0;
	}
	
	//Gera vetor do histogram
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{   
			pos = y * original->bytesperline + x * original->channels;
			if(original->data[pos] != 0) r[original->data[pos]]++;
			if(original->data[pos + 1] != 0) g[original->data[pos + 1]]++;
			if(original->data[pos + 2] != 0) b[original->data[pos + 2]]++;
		}
	
	//Calcular index com maior valor
	for(int i = 1; i < max; i++){
		if(r[i] > maior_r) maior_r = r[i];
		if(g[i] > maior_g) maior_g = g[i];
		if(b[i] > maior_b) maior_b = b[i];
	}
	
	//coloca os valores entre 0-300
	for(i = 1; i < max; i++){
		r[i] = (r[i] * 300) / maior_r;
		g[i] = (g[i] * 300) / maior_g;
		b[i] = (b[i] * 300) / maior_b;
	}
	
	//Paint
	for (y = 0; y < converted->height; y++)
		for (x = 0; x < converted->width; x++)
		{   
			pos = y * converted->bytesperline + x * converted->channels;
			
			//Paint
			//Red
			if(x == 255 || x == 510){
				converted->data[pos] = 0;
				converted->data[pos + 1] = 0;
				converted->data[pos + 2] = 0;
			}
			else if(y <= 300 && x <= 255) {
				if((320 - y) > r[x]){
						converted->data[pos] = 255;
						converted->data[pos + 1] = 255;
						converted->data[pos + 2] = 255;
				}
			}
			else if(y > 300 && x <=255) { //Bottom
						converted->data[pos] = x;
						converted->data[pos + 1] = 0;
						converted->data[pos + 2] = 0;
			}
			
			// Green
			else if(y <= 300 && x > 255 && x <= 510){
				if((320 - y) > g[x-255] && x > 255 && x <= 510){
						converted->data[pos] = 255;
						converted->data[pos + 1] = 255;
						converted->data[pos + 2] = 255;
				}
			}
			else if(y > 300 && x > 255 && x <= 510){ //Bottom
						converted->data[pos] = 0;
						converted->data[pos + 1] = x;
						converted->data[pos + 2] = 0;
			}
			
			// Blue
			else if(y <= 300 && x > 510 && x <= 765){
				if((300 - y) > b[x-510] && x > 510 && x <= 765){
						converted->data[pos] = 255;
						converted->data[pos + 1] = 255;
						converted->data[pos + 2] = 255;
				}
			}
			else if(y > 300 && x > 510 && x <= 765){ //Bottom
						converted->data[pos] = 0;
						converted->data[pos + 1] = 0;
						converted->data[pos + 2] = x;
			}	
		}
	return 1;
}

int vc_gray_histogram_equalization(IVC *original, IVC *converted){
	//VARS
	int pos, y , x, max = original->levels + 1;
	float *array;
	int n = original->width * original->height;
	
	//Vetor de histogram
	array = vc_histogram_array(original);
	
	//Calcular vetor da acumulada
	for(int i = 1; i < max; i++){
		array[i] = array[i - 1] + ((array[i] / n) * original->levels);
	}
	
	//Correr a imagem para atribuir a acumulada
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{   
			pos = y * original->bytesperline + x * original->channels;
			if(original->data[pos] != 0)
				converted->data[pos] = array[original->data[pos]];
		}
	return 1;
}

int vc_rgb_histogram_equalization(IVC *original, IVC *converted){
	//VARS
	int pos, y , x, max = original->levels + 1, i;
	float r[max],g[max],b[max];
	int n = original->width * original->height;
	
	//Inicializa array
	r[0] = 0;
	g[0] = 0;
	b[0] = 0;
	for(i = 1; i < max; i++) {
		r[i] = 0;
		g[i] = 0;
		b[i] = 0;
	}
	
	//Gera vetor do histogram
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{   
			pos = y * original->bytesperline + x * original->channels;
			if(original->data[pos] != 0) r[original->data[pos]]++;
			if(original->data[pos + 1] != 0) g[original->data[pos + 1]]++;
			if(original->data[pos + 2] != 0) b[original->data[pos + 2]]++;
		}
	
	//Calcular vetor da acumulada
	for(i = 1; i < max; i++){
		r[i] = r[i - 1] + ((r[i] / n) * original->levels);
		g[i] = g[i - 1] + ((g[i] / n) * original->levels);
		b[i] = b[i - 1] + ((b[i] / n) * original->levels);
	}
	
	//Correr a imagem para atribuir a acumulada
	for (y = 0; y < original->height; y++)
		for (x = 0; x < original->width; x++)
		{   
			pos = y * original->bytesperline + x * original->channels;
			if(original->data[pos] != 0) converted->data[pos] = r[original->data[pos]];
			if(original->data[pos + 1] != 0) converted->data[pos + 1] = g[original->data[pos + 1]];
			if(original->data[pos + 2] != 0) converted->data[pos + 2] = b[original->data[pos + 2]];
		}
	return 1;
}
#pragma endregion