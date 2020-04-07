//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT�CNICO DO C�VADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORM�TICOS
//                    VIS�O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define VC_DEBUG

#pragma region Structs

typedef struct
{
	unsigned char *data;
	int width, height;
	int channels;			// Binario/Cinzentos=1; RGB=3
	int levels;				// Binario=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline; // width * channels
} IVC;

typedef struct {
	int x, y, width, height;	// Caixa Delimitadora (Bounding Box)
	int area;					// area
	int xc, yc;					// Centro-de-massa
	int perimeter;				// Peremetro
	int label;					// Etiqueta
} OVC;

#pragma endregion

#pragma region ALOCAR E LIBERTAR UMA IMAGEM

IVC *vc_image_new(int width, int height, int channels, int levels);
IVC *vc_image_free(IVC *image);

#pragma endregion

#pragma region LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)

IVC *vc_read_image(char *filename);
int vc_write_image(char *filename, IVC *image);

#pragma endregion

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    Minhas Funções
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma region Save and Run

char *conc(const char *first, char *second);
void execute(char *filepath);
void save(char *filename, IVC * image);
void save_run(char *filename, IVC * image);

#pragma endregion

#pragma region RGB to GRAY

int vc_rgb_gray(IVC *original, IVC *converted);

#pragma endregion

#pragma region Negativos

int vc_gray_negative(IVC *original);
int vc_rgb_negative(IVC *original, IVC *converted);

#pragma endregion

#pragma region Luminosidade

int vc_image_darker(IVC *original, IVC *converted, float perc);
int vc_image_lighter(IVC *original, IVC *converted, float perc);

#pragma endregion

#pragma region Change channels

int vc_change_channels(IVC *original, IVC *converted);
int vc_only_blue(IVC *original, IVC *converted);
int vc_only_green(IVC *original, IVC *converted);
int vc_only_red(IVC *original, IVC *converted);
int vc_remove_red(IVC *original, IVC *converted);
int vc_remove_green(IVC *original, IVC *converted);
int vc_remove_blue(IVC *original, IVC *converted);

#pragma endregion

#pragma region Segmentação

int vc_hsv_segmentation(IVC *original,IVC *converted, int hmin, int hmax, int smin, int smax, int vmin, int vmax);

#pragma endregion

#pragma region Rgb to Hsv

float *min_max(float a, float b, float c);
int vc_rgb_to_hsv(IVC *original, IVC *converted);

#pragma endregion

#pragma region Exercicio aula

int vc_scale_gray_to_rgb(IVC *original, IVC *converted);

#pragma endregion

#pragma region Binary

float vc_media(IVC *original);
int vc_rgb_gray_to_binary_global_mean(IVC *original, IVC *converted);
int vc_rgb_gray_to_binary(IVC *original, IVC *converted, int threshold);
int vc_gray_to_binary_midpoint(IVC *original, IVC *converted, int kernel);
int vc_gray_to_binary_bernsen(IVC *src, IVC *dst, int kernel, int c);
int vc_gray_to_binary_niblack(IVC *original, IVC *converted, int kernel, float k);

#pragma endregion

#pragma region Normal dilate + erode

int vc_binary_dilate(IVC *original, IVC *converted, int kernel);
int vc_binary_erode(IVC *original, IVC *converted, int kernel);

#pragma endregion

#pragma region X dilate + erode

int vc_binary_erode_x(IVC *original, IVC *converted, int kernel);
int vc_gray_erode_x(IVC *original, IVC *converted, int kernel);
int vc_gray_dilate_x(IVC *original, IVC *converted, int kernel);

#pragma endregion

#pragma region open + close

int vc_binary_open(IVC *original, IVC *converted, int kernel); 
int vc_binary_close(IVC *original, IVC *converted, int kernel);

#pragma endregion

#pragma region Labeling
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                           MACROS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

OVC* vc_binary_blob_labelling(IVC *src, IVC *dst, int *nlabels);
int vc_binary_blob_info(IVC *src, OVC *blobs, int nblobs);

#pragma endregion

#pragma region Histogram

int vc_gray_histogram_show(IVC *original, IVC *converted);
int vc_gray_histogram_equalization(IVC *original, IVC *converted);
int vc_rgb_histogram_equalization(IVC *original, IVC *converted);
int vc_rgb_histogram_show(IVC *original, IVC *converted);

#pragma endregion







