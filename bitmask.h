typedef unsigned char BYTE;
typedef unsigned int DWORD;

BYTE * makeAndBitmask(BYTE * begin, BYTE width, BYTE height, unsigned int* colours)
{
	short linepadding = 0;
	if ( (linepadding = (4 - (width % 4))) == 4)
    linepadding = 0;
  short imageBytesPerLine = width + linepadding;

  short imgmasksize;
	short bytesPerLine = (width) / 8;
	bytesPerLine += bytesPerLine % 4;
	
	imgmasksize = bytesPerLine * height;
	printf("Image mask size: %u\n", imgmasksize);
	
	BYTE* imgmask = new BYTE[imgmasksize];
	unsigned short maskp = 0;
  BYTE mask = 128;

  memset(imgmask, 0, imgmasksize);

	unsigned short i = 0;
	for (short y = height - 1; y >= 0; --y)
	{
		for (short x = 0; x < width; ++x)
		{
		
			if (colours[begin[y * (imageBytesPerLine) + x]] == 0xffffff) {
				printf("| ");
				imgmask[maskp ] |= mask;
			}
			else printf("x ");
			
			if (x == width-1) {printf("\n");}
			
			mask >>= 1;
			
			if (mask == 0)
			{
				maskp ++;
				mask = 128;
			}
		}
		mask = 128;
		maskp = (++i) * bytesPerLine;
  }
  return imgmask;
}