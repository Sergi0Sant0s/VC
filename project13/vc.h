//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define VC_DEBUG

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct
{
	unsigned char *data;
	int width, height;
	int channels;			// Bin�rio/Cinzentos=1; RGB=3
	int levels;				// Bin�rio=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline; // width * channels
} IVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROT�TIPOS DE FUN��ES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUN��ES: ALOCAR E LIBERTAR UMA IMAGEM
IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

// FUN��ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Minhas Funções
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int vc_gray_negative(IVC *original);
int vc_rgb_negative(IVC *original, IVC *converted);
int vc_rgb_gray(IVC *original, IVC *converted);
int vc_image_darker(IVC *original, IVC *converted, float perc);
int vc_image_lighter(IVC *original, IVC *converted, float perc);
int vc_change_channels(IVC *original, IVC *converted);
int vc_hsv_segmentation(IVC *original,IVC *converted, int hmin, int hmax, int smin, int smax, int vmin, int vmax);
int vc_rgb_to_hsv(IVC *original, IVC *converted);
int vc_scale_gray_to_rgb(IVC *original, IVC *converted);
float vc_media(IVC *original);
int vc_rgb_gray_to_binary_global_mean(IVC *original, IVC *converted);
int vc_rgb_gray_to_binary(IVC *original, IVC *converted, int threshold);
int vc_gray_to_binary_midpoint(IVC *original, IVC *converted, int kernel);
int vc_gray_to_binary_bernsen(IVC *src, IVC *dst, int kernel, int c);
int vc_binary_dilate(IVC *original, IVC *converted, int kernel);
int vc_binary_erode(IVC *original, IVC *converted, int kernel);
float *min_max(float a, float b, float c);

int vc_only_blue(IVC *original, IVC *converted);
int vc_only_green(IVC *original, IVC *converted);
int vc_only_red(IVC *original, IVC *converted);

int vc_remove_red(IVC *original, IVC *converted);
int vc_remove_green(IVC *original, IVC *converted);
int vc_remove_blue(IVC *original, IVC *converted);
