// Hacktastic Q & D BMP -> TVGameKit sprite cruncher

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

uint8_t bmp_header[54];
uint32_t pixel_offset;
uint32_t dib_size;
int32_t bmp_width;
int32_t bmp_height;
int16_t bmp_bpp;
uint32_t bmp_line;
uint8_t *RGB24_bitmap;

#define MAX_SPRITES	256

struct sprite_info
{
	char *name;
	int32_t width;
	int32_t height;
	int32_t byte_width;
	uint8_t *mono_bitmap;
	uint8_t *mask_bitmap;
};

int32_t	num_sprites;
struct sprite_info sprite[MAX_SPRITES];

// data = (data & mask) ^ mono;
//
// mask mono 
//	0	0	= black
//	0	1	= white
//	1	0	= transparent
//	1	1	= xor
//

uint8_t human_readable = 0;
uint8_t verbose = 1;

uint8_t preamble_done = 0;

int main(int argc, char* argv[])
{
	int32_t x, y, size;
	int32_t arg;
	struct sprite_info *spr = NULL;
	
	const char* bmp_name;
	FILE* f = NULL;

	for (arg = 1; arg < argc; arg++)
	{
		spr = &sprite[num_sprites];
		
		if (argv[arg][0] == '-')
		{
			char *namestr = NULL;
			char c = argv[arg][1];
			if (c >= 'A' && c <= 'Z')
			c += 32;
			switch(c)
			{
			case 'n':
				namestr = &argv[arg][2];
				
				if (*namestr == 0 && arg+1 < argc)
				namestr = argv[++arg];
				
				if (*namestr == 0)
				{
					fprintf(stderr, "Need symbol name after -n.\n");
					exit(5);
				}

				if (spr->name)
				free(spr->name);
				spr->name = strdup(namestr);
				
				break;

			default:
				printf("Usage: crunch_bmp_rle <input BMP ...> [-n <sprite name>]\n");
				printf("\n");
				printf(" -n <name>	- Name of next tilemap or tileset font (or BMP name used)\n");
				printf("\n");
				printf("Output will go to stdout, so use \"> output\" to redirect to file.\n");
				exit(1);
				
				break;
			}
			
			continue;
		}
		
		bmp_name = argv[arg];
		
		if ((f = fopen(bmp_name, "rb")) == NULL)
		{
			fprintf(stderr, "Error opening input BMP file \"%s\".\n", bmp_name);
			exit(5);
		}
		
		if (fread(bmp_header, sizeof (unsigned char), 54, f) != 54)
		{
			fprintf(stderr, "Error reading BMP header from \"%s\".\n", bmp_name);
			exit(5);
		}
		
		if (bmp_header[0] != 'B' || bmp_header[1] != 'M')
		{
			fprintf(stderr, "Error not valid BMP format \"%s\".\n", bmp_name);
			exit(5);
		}
		
		pixel_offset = bmp_header[10] | (bmp_header[11]<<8) | (bmp_header[12]<<16) | (bmp_header[13]<<24);
		dib_size = bmp_header[14] | (bmp_header[15]<<8) | (bmp_header[16]<<16) | (bmp_header[17]<<24);
		
		if (dib_size != 40)
		{
			fprintf(stderr, "Error unsupported BMP format DIB=%d \"%s\".\n", dib_size, bmp_name);
			exit(5);
		}
		
		// extract image height and width from header
		bmp_width = bmp_header[18] | (bmp_header[19]<<8) | (bmp_header[20]<<16) | (bmp_header[21]<<24);
		bmp_height = bmp_header[22] | (bmp_header[23]<<8) | (bmp_header[24]<<16) | (bmp_header[25]<<24);
		if (bmp_width < 0)
		bmp_width = -bmp_width;
		if (bmp_height < 0)
		bmp_height = -bmp_height;
		bmp_bpp = bmp_header[28] | (bmp_header[29]<<8);
		
		if (bmp_width < 1 || bmp_width > 4096 || bmp_height < 1 || bmp_height > 4096 || bmp_bpp != 24)
		{
			fprintf(stderr, "BMP \"%s\" size or depth fails sanity check (%d x %d bpp=%d).\n", bmp_name, bmp_width, bmp_height, bmp_bpp);
			exit(5);
		}
		
		bmp_line = (((bmp_width*bmp_bpp) + 31) / 32) * 4;
		size = bmp_line * bmp_height;

		printf("\n// File: \"%s\": %d x %d (%d bpp, pixel offset %d, bytes per line %d, bitmap size %d).\n", bmp_name, bmp_width, bmp_height, bmp_bpp, pixel_offset, bmp_line, size);

		RGB24_bitmap = (uint8_t *)malloc(size);
		if (!RGB24_bitmap)
		{
			fprintf(stderr, "Failed allocating %d bytes.\n", size);
			exit(5);
		}
		memset(RGB24_bitmap, 0, size);
		
		// seek to bitmap pixels and read them in
		if (fseek(f, pixel_offset, SEEK_SET) != 0 || fread(RGB24_bitmap, sizeof (unsigned char), size, f) != (size_t)size)
		{
			fprintf(stderr, "Error reading %d bytes of BMP data from \"%s\".\n", size, bmp_name);
			exit(5);
		}
		fclose(f);
		f = NULL;
		
		// convert bitmap into sprite and fill out sprite_info
		
		// give it a symbol name if not already specified
		if (spr->name == NULL)
		{
			char str_temp[256];
			const char *n = bmp_name;

			// find start of filename after path
			if (strrchr(n, '/'))
			n = strrchr(n, '/')+1;
			else if (strrchr(n, '\\'))
			n = strrchr(n, '\\')+1;
			else if (strrchr(n, ':'))
			n = strrchr(n, ':')+1;
			
			strncpy(str_temp, n, sizeof (str_temp)-1);
			
			// remove extension
			if (strrchr(str_temp, '.'))
			*strrchr(str_temp, '.') = 0;

			spr->name = strdup(str_temp);
		}

		spr->width = bmp_width;
		spr->byte_width = ((spr->width + 7)/8)+1;
		spr->height = bmp_height;
		size = spr->byte_width * spr->height;
		spr->mono_bitmap = (uint8_t *)malloc(size);
		if (!spr->mono_bitmap)
		{
			fprintf(stderr, "Failed allocating %d bytes for mono bitmap.\n", size);
			exit(5);
		}
		memset(spr->mono_bitmap, 0, size);

		spr->mask_bitmap = (uint8_t *)malloc(size);
		if (!spr->mask_bitmap)
		{
			fprintf(stderr, "Failed allocating %d bytes for mask bitmap.\n", size);
			exit(5);
		}
		memset(spr->mask_bitmap, 0xff, size);

		printf("// BMP #%d \"%s\" (%d x %d with '.'=trans, ' '=black, 'X'=XOR, 'W'=white):\n\n", num_sprites, spr->name, spr->width, spr->height);
		printf("// *");
		for (x = 0; x < bmp_width; x++)
			printf("%c", (x & 0x7) == 0x0 ? '+' : '-');
		printf("*\n");
		
		for (y = 0; y < bmp_height; y++)
		{
			printf("// %c", (y & 0x7) == 0x0 ? '+' : '|');
			for (x = 0; x < bmp_width; x++)
			{
				uint8_t* rgb = &RGB24_bitmap[((bmp_height-1-y) * bmp_line) + (x * 3)];
				uint8_t b = rgb[1] >= 0x80 ? 0x1 : 0x0;
				uint8_t t = b ? '*' : '.';
				
					printf("%c", t);
			}
				printf("%c\n", (y & 0x7) == 0x0 ? '+' : '|');
		}
		
		printf("// *");
		for (x = 0; x < bmp_width; x++)
			printf("%c", (x & 0x7) == 0x0 ? '+' : '-');
		printf("*\n");

		printf("\n// Compressed RLE data\n");
		printf("const uint8_t %s[] PROGMEM =\n{\n\t", spr->name);

		uint32_t currun = 0;
		uint8_t curstate = 0;
		uint8_t* compout = spr->mono_bitmap;
		int tok = 0;
			
		for (y = 0; y < bmp_height; y++)
		{
			for (x = 0; x < bmp_width; x++)
			{
				uint8_t* rgb = &RGB24_bitmap[((bmp_height-1-y) * bmp_line) + (x * 3)];
				uint8_t b = rgb[1] >= 0x80 ? 0x1 : 0x0;
				
				if (b == curstate && currun < 32767)
					currun++;
				else
				{
					if (currun < 128)
					{
						*compout++ = currun;
						printf("0x%02x, ", currun);
						tok += 1;
						if (tok >= 16)
						{
							tok = 0;
							printf("\n\t");
						}
					}
					else
					{
						*compout++ = (currun >> 8) | 0x80;
						*compout++ = (currun & 0xff);

						printf("0x%02x, ", 0x80 | (currun >> 8));
						tok += 1;
						if (tok >= 16)
						{
							tok = 0;
							printf("\n\t");
						}
						printf("0x%02x, ", (currun & 0xff));
						tok += 1;
						if (tok >= 16)
						{
							tok = 0;
							printf("\n\t");
						}
					}
					currun = 1;
					curstate = !curstate;
				}
			}
		}

		if (currun)
		{
			if (currun < 128)
			{
				*compout++ = currun;
				printf("0x%02x", currun);
				tok += 1;
				if (tok >= 16)
				{
					tok = 0;
					printf("\n\t");
				}
				else
					printf(", ");
					
			}
			else
			{
				*compout++ = (currun >> 8) | 0x80;
				*compout++ = (currun & 0xff);

				printf("0x%02x", 0x80 | (currun >> 8));
				tok += 1;
				if (tok >= 16)
				{
					tok = 0;
					printf("\n\t");
				}
				else
					printf(", ");
				printf("0x%02x", (currun & 0xff));
				tok += 1;
				if (tok >= 16)
				{
					tok = 0;
					printf("\n\t");
				}
				else
					printf(", ");
			}
		}
		printf("\n};\n");
		printf("\n// Compressed RLE size %d\n",  (int32_t)(compout - spr->mono_bitmap));
		
		printf("\n// Test decompress:\n");
		
		uint8_t *cmp = spr->mono_bitmap;
		
		printf("// ");
		
		int runcnt;
		int color = 0;
		int pixcount = 0;
		while (cmp < compout)
		{
			runcnt = *cmp++;
			if (runcnt & 0x80)
				runcnt = ((runcnt & 0x7f) << 8) | *cmp++;

			while (runcnt--)
			{
				printf("%d", color);
				if (++pixcount == bmp_width)
				{
					pixcount = 0;
					printf("\n// ");
				}
			}
			color ^= 1;
		}
		
		printf("\n\n");
		
		free(RGB24_bitmap);
		RGB24_bitmap = NULL;

		num_sprites++;
	}

	printf("// EOF\n");
	fprintf(stderr, "Done!\n");

	return 0;
}
