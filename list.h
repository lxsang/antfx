/*
The MIT License (MIT)
Copyright (c) 2015 LE Xuan Sang xsang.le@gmail.com
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>

#define TYPE_NIL    0
#define TYPE_LIST   1
//NOTICE other type definition must greate than 1

#define for_each(item, list) \
            for(item = list;item!= NULL && item->type != TYPE_NIL; item = item->next)

#define list item
typedef struct __item{
	int type;
	void *value;
	struct __item* next;
}*item;

list list_init();
void list_put(list*,item);
item list_last(list);
int list_remove(list,int);
int list_size(list);
item list_at(list,int);
int list_empty(list);
item new_list_item(int type,void*);
void list_free(list *);
#endif