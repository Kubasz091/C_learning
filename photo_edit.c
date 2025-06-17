#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// BMP File Header (14 bytes)
typedef struct {
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t data_offset;
} __attribute__((packed)) BMPFileHeader;

// BMP Info Header (40 bytes for BITMAPINFOHEADER)
typedef struct {
    uint32_t header_size;    // Size of this header (40)
    int32_t width;           // Image width in pixels
    int32_t height;          // Image height in pixels
    uint16_t planes;         // Number of color planes (1)
    uint16_t bits_per_pixel; // Bits per pixel (1, 4, 8, 16, 24, 32)
    uint32_t compression;    // Compression type (0 = none)
    uint32_t image_size;     // Size of pixel data
    int32_t x_resolution;    // Horizontal resolution
    int32_t y_resolution;    // Vertical resolution
    uint32_t colors_used;    // Number of colors in palette
    uint32_t important_colors; // Important colors
}  __attribute__((packed)) BMPInfoHeader;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} RGB24Pixel;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
} __attribute__((packed)) PaletteEntry;

typedef struct {
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    PaletteEntry* palette;
    int palette_size;
    RGB24Pixel* pixels;
} BMPImage;

RGB24Pixel*read_pixels(FILE* file, BMPFileHeader* file_header, BMPInfoHeader* info_header);

RGB24Pixel* horizontal_flip(RGB24Pixel* pixels, int width, int height);
RGB24Pixel* edge_detection(RGB24Pixel* pixels, int width, int height);
RGB24Pixel* grayscale(RGB24Pixel* pixels, int width, int height);
RGB24Pixel* invert_colors(RGB24Pixel* pixels, int width, int height);

BMPImage* read_bmp(const char* filename);

void save_bmp_complete(const char* filename, BMPImage* bmp);
void free_bmp(BMPImage* bmp);



int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <bmp_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    BMPImage* bmp = read_bmp(argv[1]);
    if (!bmp) {
        printf("Error: Cannot read BMP file\n");
        return EXIT_FAILURE;
    }

    printf("=== BMP File Information ===\n");
    printf("File size: %u bytes\n", bmp->file_header.file_size);
    printf("Data offset: %u bytes\n", bmp->file_header.data_offset);
    printf("Image width: %d pixels\n", bmp->info_header.width);
    printf("Image height: %d pixels\n", bmp->info_header.height);
    printf("Bits per pixel: %u\n", bmp->info_header.bits_per_pixel);
    printf("Palette entries: %d\n", bmp->palette_size);

    printf("\n=== Image Processing Options ===\n");
    printf("1. Horizontal Flip\n");
    printf("2. Grayscale\n");
    printf("3. Edge Detection\n");
    printf("4. Invert Colors\n");
    printf("Enter your choice (1-4): ");

    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("Error: Invalid input\n");
        free_bmp(bmp);
        return EXIT_FAILURE;
    }

    RGB24Pixel* processed_pixels = NULL;
    const char* output_filename = NULL;

    switch (choice) {
        case 1:
            processed_pixels = horizontal_flip(bmp->pixels, bmp->info_header.width, abs(bmp->info_header.height));
            output_filename = "flipped_image.bmp";
            printf("Applying horizontal flip...\n");
            break;
        case 2:
            processed_pixels = grayscale(bmp->pixels, bmp->info_header.width, abs(bmp->info_header.height));
            output_filename = "grayscale_image.bmp";
            printf("Applying grayscale filter...\n");
            break;
        case 3:
            processed_pixels = edge_detection(bmp->pixels, bmp->info_header.width, abs(bmp->info_header.height));
            output_filename = "edge_detected_image.bmp";
            printf("Applying edge detection...\n");
            break;
        case 4:
            processed_pixels = invert_colors(bmp->pixels, bmp->info_header.width, abs(bmp->info_header.height));
            output_filename = "inverted_image.bmp";
            printf("Applying color inversion...\n");
            break;
        default:
            printf("Error: Invalid choice. Please select 1-4.\n");
            free_bmp(bmp);
            return EXIT_FAILURE;
    }

    if (!processed_pixels) {
        printf("Error: Failed to process image\n");
        free_bmp(bmp);
        return EXIT_FAILURE;
    }

    free(bmp->pixels);
    bmp->pixels = processed_pixels;

    save_bmp_complete(output_filename, bmp);
    printf("Processed image saved as '%s'\n", output_filename);

    free_bmp(bmp);
    return EXIT_SUCCESS;
}





RGB24Pixel* read_pixels(FILE* file, BMPFileHeader* file_header, BMPInfoHeader* info_header) {
    if (info_header->bits_per_pixel != 24) {
        printf("Error: Only 24-bit BMPs are supported\n");
        return NULL;
    }

    int width = info_header->width;
    int height = abs(info_header->height);

    int bytes_per_pixel = 3;
    int bytes_per_row = width * bytes_per_pixel;
    int padding = (4 - (bytes_per_row % 4)) % 4;

    RGB24Pixel* pixels = malloc(width * height * sizeof(RGB24Pixel));
    if (!pixels) {
        printf("Error: Cannot allocate memory for pixels\n");
        return NULL;
    }

    fseek(file, file_header->data_offset, SEEK_SET);

    for (int y = 0; y < height; y++) {
        int row_index = (height - 1 - y) * width;

        for (int x = 0; x < width; x++) {
            RGB24Pixel pixel;
            if (fread(&pixel, sizeof(RGB24Pixel), 1, file) != 1) {
                printf("Error reading pixel at (%d, %d)\n", x, y);
                free(pixels);
                return NULL;
            }
            pixels[row_index + x] = pixel;
        }

        fseek(file, padding, SEEK_CUR);
    }

    return pixels;
}

BMPImage* read_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    BMPImage* bmp = malloc(sizeof(BMPImage));
    if (!bmp) {
        fclose(file);
        return NULL;
    }

    fread(&bmp->file_header, sizeof(BMPFileHeader), 1, file);
    fread(&bmp->info_header, sizeof(BMPInfoHeader), 1, file);

    int header_size = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
    bmp->palette_size = (bmp->file_header.data_offset - header_size) / sizeof(PaletteEntry);

    if (bmp->palette_size > 0) {
        bmp->palette = malloc(bmp->palette_size * sizeof(PaletteEntry));
        if (!bmp->palette) {
            free(bmp);
            fclose(file);
            return NULL;
        }
        fread(bmp->palette, sizeof(PaletteEntry), bmp->palette_size, file);
    } else {
        bmp->palette = NULL;
    }

    bmp->pixels = read_pixels(file, &bmp->file_header, &bmp->info_header);
    if (!bmp->pixels) {
        free(bmp->palette);
        free(bmp);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return bmp;
}

void save_bmp_complete(const char* filename, BMPImage* bmp) {
    FILE* file = fopen(filename, "wb");
    if (!file) return;

    fwrite(&bmp->file_header, sizeof(BMPFileHeader), 1, file);
    fwrite(&bmp->info_header, sizeof(BMPInfoHeader), 1, file);

    if (bmp->palette && bmp->palette_size > 0) {
        fwrite(bmp->palette, sizeof(PaletteEntry), bmp->palette_size, file);
    }

    int width = bmp->info_header.width;
    int height = abs(bmp->info_header.height);
    int bytes_per_pixel = 3;
    int bytes_per_row = width * bytes_per_pixel;
    int padding = (4 - (bytes_per_row % 4)) % 4;

    for (int y = 0; y < height; y++) {
        int row_index = (height - 1 - y) * width;

        for (int x = 0; x < width; x++) {
            fwrite(&bmp->pixels[row_index + x], sizeof(RGB24Pixel), 1, file);
        }

        uint8_t pad = 0;
        for (int p = 0; p < padding; p++) {
            fwrite(&pad, 1, 1, file);
        }
    }

    fclose(file);
}

void free_bmp(BMPImage* bmp) {
    if (bmp) {
        free(bmp->pixels);
        free(bmp->palette);
        free(bmp);
    }
}

RGB24Pixel* horizontal_flip(RGB24Pixel* pixels, int width, int height) {
    RGB24Pixel* flipped = malloc(width * height * sizeof(RGB24Pixel));
    if (!flipped) return NULL;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            flipped[y * width + (width - 1 - x)] = pixels[y * width + x];
        }
    }

    return flipped;
}

RGB24Pixel* edge_detection(RGB24Pixel* pixels, int width, int height) {
    RGB24Pixel* edges = malloc(width * height * sizeof(RGB24Pixel));
    if (!edges) return NULL;

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = -pixels[(y-1) * width + (x-1)].red - 2 * pixels[y * width + (x-1)].red - pixels[(y+1) * width + (x-1)].red +
                      pixels[(y-1) * width + (x+1)].red + 2 * pixels[y * width + (x+1)].red + pixels[(y+1) * width + (x+1)].red;
            int gy = -pixels[(y-1) * width + (x-1)].red - 2 * pixels[(y-1) * width + x].red - pixels[(y-1) * width + (x+1)].red +
                      pixels[(y+1) * width + (x-1)].red + 2 * pixels[(y+1) * width + x].red + pixels[(y+1) * width + (x+1)].red;

            int magnitude = abs(gx) + abs(gy);
            if (magnitude > 255) magnitude = 255;

            edges[y * width + x].red = edges[y * width + x].green = edges[y * width + x].blue = magnitude;
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
                edges[y * width + x] = pixels[y * width + x];
            }
        }
    }

    return edges;
}

RGB24Pixel* grayscale(RGB24Pixel* pixels, int width, int height) {
    RGB24Pixel* gray_pixels = malloc(width * height * sizeof(RGB24Pixel));
    if (!gray_pixels) return NULL;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RGB24Pixel pixel = pixels[y * width + x];
            uint8_t gray_value = (uint8_t)(0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue);
            gray_pixels[y * width + x].red = gray_pixels[y * width + x].green = gray_pixels[y * width + x].blue = gray_value;
        }
    }

    return gray_pixels;
}

RGB24Pixel* invert_colors(RGB24Pixel* pixels, int width, int height) {
    RGB24Pixel* inverted = malloc(width * height * sizeof(RGB24Pixel));
    if (!inverted) return NULL;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RGB24Pixel pixel = pixels[y * width + x];
            inverted[y * width + x].red = 255 - pixel.red;
            inverted[y * width + x].green = 255 - pixel.green;
            inverted[y * width + x].blue = 255 - pixel.blue;
        }
    }

    return inverted;
}

