#include <stdio.h>
#include <stdlib.h>

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned long LONG; 
typedef unsigned char BYTE;

int lc=0; 

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
struct BITMAPFILEHEADER
{
	WORD	bfType;  
	DWORD	bfSize;  
	WORD	bfReserved1;  
	WORD	bfReserved2;  
	DWORD	bfOffBits;  
};
#pragma pack(pop)   /* restore original alignment from stack */

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
struct BITMAPINFOHEADER
{
	DWORD	biSize;  
	LONG	biWidth;  
	LONG	biHeight; 
	WORD	biPlanes; 
	WORD	biBitCount; 
	DWORD	biCompression;
	DWORD	biSizeImage;  //co the ==0 khi ko bi nen (compression la BI_RGB)
	LONG	biXPelsPerMeter;  
	LONG	biYPelsPerMeter;  
	DWORD	biClrUsed;  //==0 khi su dung het 
	DWORD	biClrImportant;  
	//neu la anh 8bit: bfSize = biSizeImage + 54(Header) + 0->1024(palette) 
};
#pragma pack(pop)   /* restore original alignment from stack */

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
struct RGBQUAD {
	BYTE    rgbBlue; 
	BYTE    rgbGreen; 
	BYTE    rgbRed; 
	BYTE    rgbReserved; 
};
#pragma pack(pop)   /* restore original alignment from stack */


int copy(unsigned char *bitmapData, BITMAPFILEHEADER *bitmapFileHeader, BITMAPINFOHEADER *bitmapInfoHeader, RGBQUAD *Palette) {
	FILE *fp = fopen("Output.bmp","wb");
	if (fp == NULL)
		return -1;
	fwrite(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,fp);
	LONG w = bitmapInfoHeader->biWidth;
	LONG h = bitmapInfoHeader->biHeight;
	DWORD Size_Padd;
	if (bitmapInfoHeader->biBitCount>=16) {
		if ((w*3)%4==0) { Size_Padd = w*h*3; }
		else { Size_Padd = (w*3 + (4 - ((w*3)%4)))*h; }
	}
	else {
		if (w%4==0) { Size_Padd = w*h; }
		else { Size_Padd = (w + (4 - (w%4)))*h; }
		DWORD mau;
		if (bitmapInfoHeader->biClrUsed==0) { mau = 256; }
		else { mau = bitmapInfoHeader->biClrUsed; }
		fwrite(Palette, sizeof(RGBQUAD),mau,fp);
	}
	fwrite(bitmapData,Size_Padd ,1,fp);
	fclose(fp);
	return 0;
}

int ktra(int i, int j, int w, int h) {
	if (i<0 || i>=h || j<0 || j>=w) return 0;
	return 1;
}

int cut(unsigned char *bitmapData, BITMAPFILEHEADER *bitmapFileHeader, BITMAPINFOHEADER *bitmapInfoHeader, RGBQUAD *Palette) {
	FILE *fp = fopen("Convert.bmp","wb");
	if (fp == NULL)
		return -1;
	int m,n;
	printf("Nhap m n: ");
	scanf("%d%d", &m, &n);
	LONG w = bitmapInfoHeader->biWidth;
	LONG h = bitmapInfoHeader->biHeight;
	int **a, *b;
	int i,j; 
	a = (int**) malloc (sizeof(int*)*h);
	b = (int*) malloc (sizeof(int*)*h*w);
	for (i=0; i<h; i++)
		a[i] = b + i*w;
	for (i=0; i<h; i++) //khoi tao
		for (j=0; j<w; j++)
			a[i][j]=1;
	int temp;
	int left, right, top, bot; // bien cua nhung diem thuoc anh 
	int left2, right2, top2, bot2; // bien cua tat ca cac diem (thuoc va ca ko thuoc)
// lay bien trai:
	temp = (w/2) - (n/2);
	if (temp<0) { left = 0; left2 = temp; }
	else { left = temp; left2 = 0; }
// lay bien phai:
	temp = (w-(w/2)) - (n-(n/2));
	if (temp<0) { 
		right = w;  // lat se de dau < nen ta gan = w
		right2 = n + left2;
	}
	else { right = w-temp; right2 = w; }
// lay bien tren:
	temp = (h/2) - (m/2);
	if (temp<0) { top = 0; top2 = temp; }
	else { top = temp; top2 = 0; }
// lay bien duoi: 
	temp = (h-(h/2)) - (m-(m/2));
	if (temp<0) { 
		bot = h;
		bot2= m + top2;
	}
	else { bot = h-temp; bot2 = h; }
	for (i=top; i<bot; i++)
		for (j=left; j<right; j++)
			a[i][j]=2;
//chinh sua thong tin trong header, xuat header:
	bitmapInfoHeader->biWidth = n;
	bitmapInfoHeader->biHeight = m;
	int index = -1; //khai bao o trong if thi o ben ngoai if se ko dung dc 
	if (bitmapInfoHeader->biBitCount>=16) { // neu la anh >=16 bit
		if (bitmapInfoHeader->biSizeImage!=0) {
			if ((n*3)%4==0) { bitmapInfoHeader->biSizeImage = n*m*3; }
			else { bitmapInfoHeader->biSizeImage = (n*3 + (4 - ((n*3)%4)))*m; }
		}
		if ((n*3)%4==0) { bitmapFileHeader->bfSize = n*m*3 + 54;	}
		else {bitmapFileHeader->bfSize = (n*3 + (4 - ((n*3)%4)))*m + 54; }
		fwrite(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,fp);
		fwrite(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,fp);
	}
	else { //neu la anh <= 8 bit
		if (bitmapInfoHeader->biSizeImage!=0) {
			if (n%4==0) { bitmapInfoHeader->biSizeImage = n*m; }
			else { bitmapInfoHeader->biSizeImage = (n + (4 - (n%4)))*m; }
		}
		if (n%4==0) { bitmapFileHeader->bfSize = n*m + 54;	}
		else {bitmapFileHeader->bfSize = (n + (4 - (n%4)))*m + 54; }
		//Palette:
		DWORD mau;
		if (bitmapInfoHeader->biClrUsed==0) { mau = 256; }
		else { mau = bitmapInfoHeader->biClrUsed; }
		for (i=0; i<mau; i++) {
			if (Palette[i].rgbBlue==0 && Palette[i].rgbGreen==0 &&
				Palette[i].rgbRed==0) {
					index = i;
					break;
			}
		}
		if (index==-1) {
			index = mau;
			mau++;
			if (bitmapInfoHeader->biClrUsed!=0) {
				if (mau==256) bitmapInfoHeader->biClrUsed = 0;
				else bitmapInfoHeader->biClrUsed = mau;
			}
			Palette[mau-1].rgbBlue=0;
			Palette[mau-1].rgbGreen=0;
			Palette[mau-1].rgbRed=0;
			Palette[mau-1].rgbReserved=0;
			bitmapFileHeader->bfOffBits = bitmapFileHeader->bfOffBits + 4;
		}
		fwrite(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,fp);
		fwrite(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,fp);
		fwrite(Palette, sizeof(RGBQUAD),mau,fp);
	}
//=======================================================
//<=8 bit:
	int count = 0;
	int padd;
	int r = 0;
	int lc;
	if (bitmapInfoHeader->biBitCount<=8) {
		for (i=top2; i<bot2; i++) {
			lc = 0;
			for (j=left2; j<right2; j++) {
				if (ktra(i,j,w,h)==1) { //neu diem do thuoc cai anh 
					if (a[i][j]==2) {
						fwrite(&bitmapData[count], 1, 1, fp);  
						lc = 1;
					}
					count++;
				}
				else {
					if ((left<=j && j<right) || (top<=i && i<bot) ||
						(left2<0 && right2>=w && top2<0 && bot2>=h)) { 
						fwrite(&index, 1, 1, fp);
						lc = 1;
					}
				}
			}
			if (0<=i && i<h) {
				if (w%4==0) { padd = 0;	}
				else { padd = 4 - (w%4); 
					   count = count + padd;
				}
			}
			if (lc==1) {
				if (n%4==0) { padd = 0;	}
				else { padd = 4 - (n%4); 
					   fwrite(&r, padd, 1, fp); 
				}
			}
		}
	}
//>=16 bit:
	if (bitmapInfoHeader->biBitCount>=16) {
		for (i=top2; i<bot2; i++) {
			lc = 0;
			for (j=left2; j<right2; j++) {
				if (ktra(i,j,w,h)==1) { //neu diem do thuoc cai anh 
					if (a[i][j]==2) {
						fwrite(&bitmapData[count], 3, 1, fp);  
						lc = 1;
					}
					count+=3;
				}
				else {
					if ((left<=j && j<right) || (top<=i && i<bot)||
						(left2<0 && right2>=w && top2<0 && bot2>=h)) { 
						fwrite(&r, 3, 1, fp);
						lc = 1;
					}
				}
			}
			if (0<=i && i<h) {
				if ((w*3)%4==0) { padd = 0;	}
				else { padd = 4 - ((w*3)%4); 
					  count = count + padd;
				}
			}
			if (lc==1) {
				if ((n*3)%4==0) { padd = 0;	}
				else { padd = 4 - ((n*3)%4); 
					fwrite(&r, 1, padd, fp); 
				}
			}
		}
	}
	free(b);
	free(a);
	fclose(fp);
	return 0;
}

unsigned char* LoadBitmapFile(char *filename, BITMAPFILEHEADER *bitmapFileHeader, BITMAPINFOHEADER *bitmapInfoHeader, RGBQUAD *&Palette) {
	FILE *filePtr; 
	filePtr = fopen(filename,"rb");
	if (filePtr == NULL)
		return NULL;
	
	fread(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);
	if (bitmapFileHeader->bfType !=0x4D42) // hoac la 19778
	{
		fclose(filePtr);
		return NULL;
	}

	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr);
	if (bitmapInfoHeader->biCompression!=0) {
		printf("Ma nguon nay khong ho tro cho file anh bi nen\n");
		lc=1;
		return NULL;
	}

	LONG w = bitmapInfoHeader->biWidth;
	LONG h = bitmapInfoHeader->biHeight;
	DWORD Size_Padd;
	unsigned char *bitmapImage; 
//16 24 32 bit: 
	if (bitmapInfoHeader->biBitCount>=16) {
		if ((w*3)%4==0) { Size_Padd = w*h*3; }
		else { Size_Padd = (w*3 + (4 - ((w*3)%4)))*h; }
		//Size_Padd: kich thuoc co tinh ca padding 
	}
	else { // 1 4 8 bit:
		if (w%4==0) { Size_Padd = w*h; }
		else { Size_Padd = (w + (4 - (w%4)))*h; }
		DWORD mau;
		if (bitmapInfoHeader->biClrUsed==0) { mau = 256; }
		else { mau = bitmapInfoHeader->biClrUsed; }
		Palette = (RGBQUAD*) malloc (sizeof(RGBQUAD)*mau + 1); //tao du ra 1 chut lat co viec can xai
		fread(Palette, sizeof(RGBQUAD),mau,filePtr);
	}
	fseek(filePtr, bitmapFileHeader->bfOffBits, SEEK_SET);
	bitmapImage = (unsigned char*) malloc (Size_Padd);
	if (!bitmapImage) //neu ko du bo nho de cung cap (==NULL)
	{					//((not) bitmapImage)
		free(bitmapImage);
		printf("Khong du bo nho de load anh nay");
		lc=1;
		fclose(filePtr);
		return NULL;
	}
	fread(bitmapImage,Size_Padd,1,filePtr);
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}
	fclose(filePtr);
	return bitmapImage;
}

void main(int argc, char *argv[]) {
	unsigned char *bitmapData;
	BITMAPFILEHEADER bitmapFileHeader; 
	BITMAPINFOHEADER bitmapInfoHeader;
	RGBQUAD *Palette;
//load:
	bitmapData = LoadBitmapFile(argv[1], &bitmapFileHeader, &bitmapInfoHeader, Palette);
	if (bitmapData==NULL) {
		if (lc==0) printf("Du lieu vao khong hop le\n");
		return;
	}
//copy:
	int k;
	k = copy(bitmapData, &bitmapFileHeader, &bitmapInfoHeader, Palette);
	if (k==-1) {
		printf("Khong the tao file de copy\n");
		return;
	}
//cut: 
	k = cut(bitmapData, &bitmapFileHeader, &bitmapInfoHeader, Palette );
	if (k==-1) {
		printf("Khong the tao file de convert\n");
		return;
	}
	free(bitmapData); 
}