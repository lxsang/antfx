#include "image.h"
void read_bitmap_file(const char* file, afx_bitmap_t* bmp)
{
    uint8_t header[14];
    FILE *fp; 
    int i=0;  
    unsigned char tmpRGB; 

    fp = fopen(file,"rb");
    if (fp == NULL)
        return;

    fread(header, 14,1,fp);
    
    if(*((uint16_t*)header) !=0x4D42)
    {
        fclose(fp);
        LOG("wrong magic number \n");
        return;
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
        return;
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

void _draw_bitmap(afx_bitmap_t bmp,point_t tr)
{
    int i;
    pixel_t c;
    point_t p;
    //int line_length = bmp.header.width*3;
    for(i=0;i < bmp.header.size;i+=3)
    {
        c.b = bmp.data[i];
        c.g = bmp.data[i+1];
        c.r = bmp.data[i+2];
        p.x = bmp.header.width - ((i/3) % bmp.header.width) -1;
        p.y = bmp.header.height - ((i/3) / bmp.header.width) - 1;
        //LOG("x %d  y %d : %d\n", p.x,p.y);
        //break;
        _put_pixel(_T(tr,p),COLOR(c));
    }
}