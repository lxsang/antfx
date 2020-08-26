#include "image.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>
int read_bitmap_file(const char* file, afx_bitmap_t* bmp)
{
    uint8_t header[14];
    FILE *fp;  
    bmp->loaded = 0;
    fp = fopen(file,"rb");
    if (fp == NULL)
        return 0;

    fread(header, 14,1,fp);
    
    if(*((uint16_t*)header) !=0x4D42)
    {
        fclose(fp);
        LOG("wrong magic number \n");
        return 0;
    }

    //read the bitmap info header
    fread(&bmp->header, sizeof(afx_bitmap_tag_t),1,fp); 

    //move file point to the begging of bitmap data
    //LOG("data offset is %d\n",*((uint32_t*)(header+10)));
    fseek(fp, *((uint32_t*)(header+10)), SEEK_SET);

    //allocate enough memory for the bitmap image data
    bmp->data = (uint8_t*)malloc(bmp->header.size);

    //verify memory allocation
    if (!bmp->data)
    {
        fclose(fp);
        return 0;
    }

    //read in the bitmap image data
    fread(bmp->data,bmp->header.size,1,fp);


    //swap the r and b values to get RGB (bitmap is BGR)
    /*for (i = 0;i < bmp->header.size;i+=3) // fixed semicolon
    {
        tmpRGB = bmp->data[i];
        bmp->data[i] = bmp->data[i + 2];
        bmp->data[i + 2] = tmpRGB;
    }*/

    //close file and return bitmap iamge data
    fclose(fp);
    bmp->loaded = 1;
    return 1;
}

void release_bitmap(afx_bitmap_t* bmp)
{
    bmp->loaded = 0;
    if(bmp->data)
    {
        free(bmp->data);
        bmp->data= NULL;
    }
}
void dump_bitmap(const char* file)
{
    afx_bitmap_t bmp;
    read_bitmap_file(file,&bmp);

    LOG("hsize:%d\n", bmp.header.hsize);
    LOG("w:%d\n", bmp.header.width);
    LOG("h:%d\n", bmp.header.height);
    LOG("planes: %d\n", bmp.header.planes);
    LOG("bbp: %d\n", bmp.header.bbp);
    LOG("compression %d \n",bmp.header.compression);
    LOG("size %d \n", bmp.header.size);
}