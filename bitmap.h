// bitmap header

// 14 bytes
struct BitmapHeader { // all bitmaps files(.bmp) have this header
	unsigned short type; // 0x4d42 for bitmaps
	unsigned int size;
	unsigned int reserved;
	unsigned int offset;
};

// 40 bytes
struct DIBitmapHeader { // all bitmaps have this header. also known as the info header.
	unsigned int size;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bpp;
	unsigned int comprmeth;
	unsigned int imgsize;
	unsigned int horppm;
	unsigned int verppm;
	unsigned int colours;
	unsigned int used;
};

// 6 bytes
struct IconDir { // icon directory. header for an icon file(.ico).
	unsigned short reserved;
	unsigned short type;
	unsigned short count;
};

// 16 bytes
struct IconEntry { // icon entry. info about an icon in the icon file(.ico).
	unsigned char width;
	unsigned char height;
	unsigned char colours;
	unsigned char reserved;
	unsigned short planes;
	unsigned short bpp;
	unsigned int size;
	unsigned int offset;
};
