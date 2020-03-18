#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <assert.h>


//------------------------------------------------
class Rothstein
{
public:
	Rothstein(int p, int q);
	int operator[] (int i) { return myData[i]; }
	void Permute();
	void PrintData();

	~Rothstein();


private:
	char *myData;
	int myP, myQ;
};

class RowRothstein280
{
public:
	RowRothstein280(int largerSize, int bpp); //increment according to byte per pixel
	int operator[] (int i) { return myData[i]; }
	void PrintData();
private:
	int myData[280];
};

class ColRothstein210
{
public:
	ColRothstein210(int largerSize, int bpl); //increment according to byte perl line 
	int operator[] (int i) { return myData[i]; }
	void PrintData();

private:
	int myData[210];
};

Rothstein::Rothstein(int p, int q)
{
	myP = p;
	myQ = q;
	myData = new char[p];

	if (p <= q) {
		memset(myData, 1, p);
		return;
	}

	char *ptr = myData;
	int diff = p - q;
	int  curr = q;
	for (int i = 0; i< p; i++)     //if we want to permute the code, we need <<1
	{
		if (curr < p)
		{
			*ptr = 0;
			curr += q;
		}
		else
		{
			*ptr = 1;
			curr -= diff;
		}
		ptr++;
	}
}

void Rothstein::Permute()
{
	int temp = myData[0];

	for (int i = 0; i<myP - 1; i++)
		myData[i] = myData[i + 1];

	myData[myP - 1] = temp;
}

void Rothstein::PrintData()
{

	for (int i = 0; i<myP; i++)
	{
		printf("%d\t", myData[i]);
	}
	printf("\n");
}

Rothstein::~Rothstein()
{
	delete myData;
}

//Rothstein of size 280, for Row 
// bpp = byte per pixel
RowRothstein280::RowRothstein280(int largerSize, int bpp)
{
	//    assert(largerSize > 280);

	Rothstein rot(largerSize, 280);
	int j = 0;
	int oldData = 0;
	for (int i = 0; i< largerSize; i++)
	{
		if (rot[i])
		{
			myData[j++] = (i - oldData)*bpp; //increment in bytes per pixel * number of pixels
			oldData = i;
		}
	}
	//printf("j is %d \n", j);

}

void RowRothstein280::PrintData()
{

	for (int i = 0; i<280; i++)
	{
		printf("%d\t", myData[i]);
	}
	//    printf("\n");
}

ColRothstein210::ColRothstein210(int largerSize, int bpl)
{
	//    assert(largerSize > 210);

	Rothstein rot(largerSize, 210);
	int j = 0;
	int oldData = 0;
	for (int i = 0; i< largerSize; i++)
	{
		if (rot[i])
		{
			myData[j++] = (i - oldData) * bpl; //increment by number of lines * bytes per line
			oldData = i;
		}
	}
	//    printf("j is %d \n", j);

}

void ColRothstein210::PrintData()
{

	for (int i = 0; i<210; i++)
	{
		printf("%d\t", myData[i]);
	}
	printf("\n");
}

/*

HeavyRothstein

If we are shrinking an image from a columns to b columns, we are interested
in the Rothstein code for p = a, q = b.

If we are expanding an image from a columns to b columns, we are interested
in the Rothstein code for p = b, q = a.

HeavyRothstein takes two dimensions and automatically constructs a Rothstein
code with p set to the larger dimension and q set to the smaller dimension.

*/

//int minval(int a, int b) { return a < b ? a : b; }
//int maxval(int a, int b) { return a > b ? a : b; }

class HeavyRothstein : public Rothstein
{
public:
	HeavyRothstein(int a, int b) : Rothstein(max(a, b), min(a, b)) {}
};


BYTE* bits = NULL;
BYTE* newbits = NULL;
FILE* file = NULL;
BITMAPFILEHEADER bf;//位图文件头
unsigned newwidth, newheight, newimagesize, newbfsize;
unsigned bmiSize;
unsigned imageSize;
int bpp, bpl;
RowRothstein280 *rot280;
ColRothstein210 *rot210;

void InitDevice()
{
	//-------800x600 to 280x210
	newwidth = 280;
	newheight = 210;

	//-------byte per pixel 3   original image byte per line 800 x 3 = 2400
	bpp = 3;
	bpl = 800 * bpp; //2400

	rot280 = new RowRothstein280(800, bpp);
	rot210 = new ColRothstein210(600, bpl);

}



//from 800x600 to 280x210
void ResizeImage800to280(unsigned char* src, unsigned char* dst)
{
	unsigned char* oldp;  //pointer to the pix in the old image
	int oldcol = 0;

	for (int row = 0; row < 210; row++)
	{
		src += (*rot210)[row];
		oldp = src;
		for (int col = 0; col < 280;)
		{    //unroll 10 times;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
			oldp += (*rot280)[col++];
			memcpy(dst, oldp, bpp);
			dst += bpp;
		}
	}
}

//-----------------------end 800x600 to 280x210 bpp=3--------------------------------------------------------



struct {
	BITMAPINFOHEADER bmiHeader;//包含了有关位图的尺寸及位格式等信息
	RGBQUAD bmiColors[256];//说明彩色表RGBQUAD结构的阵列，其中包含索引图像的真实RGB值
} bmi;//位图信息

void LoadBMP(char* filename)
{

	if (!(file = fopen(filename, "rb"))) {
		goto done;
	}

	if (fread(&bf, 1, sizeof(bf), file) != sizeof(bf)) {
		goto done;
	}

	if (bf.bfType != *(WORD*) "BM") {//判断是否是bmp文件，bmp文件位图文件头的bfType一定是"BM"(0x4D42)
		goto done;
	}

	bmiSize = bf.bfOffBits - sizeof(bf);//bfOffBits:从文件头开始到实际的图象数据之间的字节的偏移量
	if (bmiSize > sizeof(bmi)) {
		goto done;
	}
	if (fread(&bmi, 1, bmiSize, file) != bmiSize) {
		goto done;
	}
	//biWidth:图像宽度，以像素为单位 
	/* biHeight：图象的高度，以象素为单位。如果该值是一个正数，说明图像是倒向的，如果该值是一个负数，则说明图像是正向的。
	当高度值是一个负数时（正向图像），图像将不能被压缩 */
	//biBitCount：说明比特数/象素，其值为1、4、8、16、24、或32
	imageSize = (bmi.bmiHeader.biWidth * bmi.bmiHeader.biBitCount / 8 + 3 & ~3) * bmi.bmiHeader.biHeight;
	bits = new BYTE[imageSize];


	if (fread(bits, 1, imageSize, file) != imageSize) {
		goto done;
	}

	printf("image size is %d\n", imageSize);
	printf("color is %d\n", bmi.bmiHeader.biBitCount);
	printf("width is %d height is %d\n", bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight);

	//image = LoadBMP((BITMAPINFO*) &bmi, bits);

done:
	if (file) {
		fclose(file);
	}

}

// CopyPixel will copy a pixel

inline void CopyPixel(unsigned char *dst, unsigned char *src, int byteperpix)
{
	for (int i = 0; i<byteperpix; i++)
		*dst++ = *src++;
}


//regular Rothstein algorithm
void ResizeImage(unsigned char* src, int sw, int sh, int sp, unsigned char* dst, int dw, int dh, int bpp)
{
	unsigned char* oldp;  //pointer to the pix in the old image
	HeavyRothstein rothrow(dw, sw);
	HeavyRothstein rothcol(dh, sh);
	int oldcol = 0;

	for (int col = 0; col < dh; col++)
	{
		// If shrinking in height, find a 1 in the Rothstein code
		if (dh < sh) {
			while (rothcol[oldcol] == 0) {
				oldcol++;
				src += sp;
			}
		}

		// Resize a single row of the image.   
		oldp = src;

		int oldrow = 0;

		for (int row = 0; row < dw; row++)
		{
			// If shrinking in width, find a 1 in the Rothstein code
			if (dw < sw) {
				while (rothrow[oldrow] == 0) {
					oldrow++;
					oldp += bpp;
				}
			}

			CopyPixel(dst, oldp, bpp);
			dst += bpp;

			// Advance the "old" pointer
			// the # of cols specified in the Rothstein code
			if (dw < sw) {
				oldrow++;
				oldp += bpp;
			}
			else {
				oldrow += rothrow[row];
				oldp += rothrow[row] * bpp;
			}
		}

		// Advance the "old" pointer the # of rows specified in the Rothstein code
		if (dh < sh) {
			oldcol++;
			src += sp;
		}
		else {
			oldcol += rothcol[col];
			src += sp * rothcol[col];
		}
	}
}

void ProcessBMP(char* filename)
{

	//    HeavyRothstein rotrow(bmi.bmiHeader.biWidth, newwidth);
	//    HeavyRothstein rotcol(bmi.bmiHeader.biHeight, newheight);

	newimagesize = (newwidth* bmi.bmiHeader.biBitCount / 8 + 3 & ~3)* newheight;

	newbits = new BYTE[newimagesize];

	int bytes_per_line = bmi.bmiHeader.biBitCount / 8 * bmi.bmiHeader.biWidth + 3 & ~3;

	//    ResizeImage((unsigned char*) bits, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, bytes_per_line, (unsigned char*) newbits, newwidth, newheight, bmi.bmiHeader.biBitCount / 8);


	ResizeImage800to280((unsigned char*)bits, (unsigned char*)newbits);

	int filesize = sizeof(bf)+bmiSize + newimagesize;
	bf.bfSize = filesize;
	bmi.bmiHeader.biWidth = newwidth;
	bmi.bmiHeader.biHeight = newheight;
	bmi.bmiHeader.biSizeImage = newimagesize;


	if (!(file = fopen(filename, "wb"))) {
		goto done;
	}
	if (fwrite(&bf, 1, sizeof(bf), file) != sizeof(bf)) {
		goto done;
	}

	if (fwrite(&bmi, 1, bmiSize, file) != bmiSize) {
		goto done;
	}

	if (fwrite(newbits, 1, newimagesize, file) != newimagesize) {
		goto done;
	}


done:
	if (file) {
		fclose(file);
	}
	if (bits) {
		delete[] bits;
	}
	if (newbits) {
		delete[] newbits;
	}

	if (rot280) {
		delete rot280;
	}
	if (rot210) {
		delete rot210;
	}

}

int main(int argc, char* argv[])
{
	char* infile;
	char* outfile;

	//if(argc > 2)
	{
		/*
		infile = "test.bmp";
		newwidth = 800;
		newheight = 600;
		outfile = "test1.bmp"; //argv[2];
		*/
		/*
		infile = "wallpaper1.bmp"; //argv[1];
		newwidth = 640;
		newheight = 480;
		outfile = "test.bmp"; //argv[2];
		*/
		infile = "test.bmp"; //argv[1];
		outfile = "test2.bmp"; //argv[2];

		InitDevice();

		printf("in is %s\n", infile);
		printf("out is %s\n", outfile);
		LoadBMP(infile);
		ProcessBMP(outfile);
	}

	//    HeavyRothstein rot(800, 280);
	//    rot.PrintData();
	//    rot280.PrintData();
	printf("Hello World!\n");
	//    rot210.PrintData();

	//    HeavyRothstein rot1(600, 210);
	//    rot1.PrintData();


	return 0;
}