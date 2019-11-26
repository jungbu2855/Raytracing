#pragma once

// Definitions
typedef struct {
	unsigned char x, y, z, w;
} uchar4;

typedef unsigned char uchar;

// Functions
void LoadBMPFile(uchar4 **dst, int *width, int *height, const char *name);
void SaveBMPFile(uchar4 *dst, int width, int height, const char *outputname, const char *inputname);