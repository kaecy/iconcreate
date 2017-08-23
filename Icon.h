#include <stdio.h>
#include <string.h>
#include "bitmap.h"
#include "bitmask.h"
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;

struct BITMAP {
	BYTE* bytes;
	BYTE* data;
	WORD size;
	WORD width;
	WORD height;
};

class Icon {
	BYTE* img;
	DWORD* clr;
	IconDir dir;
	IconEntry entry;
	DIBitmapHeader header;
	
public:
	Icon(char fname[]);
	~Icon();
	
	void saveIconToBMP();
	short count();
	static void createICOFromBMP(char*[], BYTE count);
};

Icon::Icon(char fname[])
{
	FILE* fd = fopen(fname, "rb");
	// read the icon dir
	fread(&dir.reserved, 2, 1, fd);
	fread(&dir.type, 2, 1, fd);
	fread(&dir.count, 2, 1, fd);
	// read the icon entry
	fread(&entry, sizeof(entry), 1, fd);
	// read the img data
	fseek(fd, entry.offset, SEEK_SET);
	fread(&header, sizeof(header), 1, fd);
	// read the colour table
	clr = new DWORD[header.colours];
	fread(clr, header.colours * 4, 1, fd);
	// read raw img data
	img = new BYTE[header.imgsize];
	fread(img, header.imgsize, 1, fd);
	fclose(fd);
}

Icon::~Icon()
{
	delete[] img;
	delete[] clr;
}

short Icon::count()
{
	return dir.count;
}

void Icon::saveIconToBMP()
{
	size_t colours_size = entry.colours * 4;
	
	printf("Width: %u\nHeight: %u\n", entry.width, entry.height);
	printf("BitsPerPixel: %u\n", entry.bpp);
	printf("Colour Ary: %ub\n", colours_size);
	
	header.height = entry.height;
	header.imgsize = (entry.width * entry.height * header.bpp) / 8;
	printf("Image Ary: %ub\n", header.imgsize);
	
	BitmapHeader bmheader;
	bmheader.type = 0x4d42;
	bmheader.size = 54 + colours_size + header.imgsize;
	bmheader.reserved = 0;
	bmheader.offset = 54 + colours_size;
	
	FILE* outfd = fopen("raw.bmp", "wb");
	
	fwrite(&bmheader.type, 2, 1, outfd);
	fwrite(&bmheader.size, 4, 1, outfd);
	fwrite(&bmheader.reserved, 4, 1, outfd);
	fwrite(&bmheader.offset, 4, 1, outfd);
	
	fwrite(&header, sizeof(header), 1, outfd);
	clr[0] = 0x00ffffff; // change the background colour to white before saving the 
	                      // colour table
	fwrite(clr, 4 * header.colours, 1, outfd);
	fwrite(img, header.imgsize, 1, outfd);
	fclose(outfd);
}

void Icon::createICOFromBMP(char * fnames[], BYTE count)
{
	FILE* fd;
	BITMAP* bitmaps;

	bitmaps = new BITMAP[count];

	// read bitmaps
	BitmapHeader bmheader;
	DIBitmapHeader* header;

	for (int i = 0; i < count; i ++)
	{
		fd = fopen(fnames[i], "rb");
    
    if (fd == 0)
      return;

		fread(&bmheader.type, 2, 1, fd);
		fread(&bmheader.size, 4, 1, fd);
		fread(&bmheader.reserved, 4, 1, fd);
		fread(&bmheader.offset, 4, 1, fd);

    printf("Bitmap #%d\n", i+1);
		printf("Type: %x\n", bmheader.type);
		printf("File Size: %u\n", bmheader.size);

		bitmaps[i].size = bmheader.size - 14;
    bitmaps[i].bytes = new BYTE[bitmaps[i].size];
    fread(bitmaps[i].bytes, bitmaps[i].size, 1, fd);
		header = (DIBitmapHeader*) bitmaps[i].bytes;
		bitmaps[i].width = header->width;
    bitmaps[i].height = header->height;
    printf("Size: %dx%d\n\n", bitmaps[i].width, bitmaps[i].height);
    bitmaps[i].data = bitmaps[i].bytes + bmheader.offset - 14;
		fclose(fd);
	}
	// we have all the bitmap data
	
	// create the bitmasks
	BITMAP* bitmasks = new BITMAP[count];

	for (int i = 0; i < count; i++)
	{
		bitmasks[i].bytes = 
			makeAndBitmask(
				bitmaps[i].data,
				bitmaps[i].width,
				bitmaps[i].height,
				(unsigned int*) (bitmaps[i].bytes + 40)
		);

		short bytesPerLine = bitmaps[i].width / 8;
		bytesPerLine += bytesPerLine % 4;
		bitmasks[i].data = (BYTE *)bytesPerLine;
		bitmasks[i].size = bytesPerLine * bitmaps[i].height;

		bitmasks[i].width = bitmaps[i].width;
		bitmasks[i].height = bitmaps[i].height;
	}

	// let's create the ico file
	
	fd = fopen("out.ico", "wb");
	IconDir dir;
	IconEntry* entry = new IconEntry[count];
	
	dir.reserved = 0;
	dir.count = count;
	dir.type = 1;

	WORD nextEntryOffset = 6 + sizeof(IconEntry) * count;
	for (int i = 0; i < count; i++)
	{
		entry[i].width = bitmaps[i].width;
		entry[i].height = bitmaps[i].height;
		entry[i].colours = 0;
		entry[i].reserved = 0;
		entry[i].planes = 1;
		entry[i].bpp = 8;
		entry[i].size = bitmaps[i].size + bitmasks[i].size;
		entry[i].offset = nextEntryOffset;
		nextEntryOffset += entry[i].size;
	}
	

	fwrite(&dir.reserved, 2, 1, fd);
	fwrite(&dir.type, 2, 1, fd);
	fwrite(&dir.count, 2, 1, fd);

	fwrite(entry, sizeof(IconEntry), count, fd);
  delete[] entry;

	for (int i = 0; i < count; i++)
	{
		header = (DIBitmapHeader*)bitmaps[i].bytes;
		header->height = header->height * 2;
		
		fwrite(bitmaps[i].bytes, bitmaps[i].size, 1, fd); // write bitmap
		
		short height = bitmasks[i].height;
		for (short y = height - 1; y >= 0; --y)	// write bitmask
			fwrite(&bitmasks[i].bytes[y * (int)bitmasks[i].data], (size_t)bitmasks[i].data, 1, fd);

		delete[] bitmaps[i].bytes;
		delete[] bitmasks[i].bytes;
	}
	delete[] bitmaps;
	delete[] bitmasks;
}
