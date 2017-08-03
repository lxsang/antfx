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
#include "list.h" 

list list_init()
{
	list ret = (list)malloc(sizeof *ret); 
	ret->type = TYPE_NIL;
	ret->next = NULL;
	return ret;
}
void list_put(list* l, item it)
{
	if(*l == NULL || (*l)->type == TYPE_NIL)
	{
		*l = it;
		return ;
	}	
	item np = list_last(*l);
	np->next = it;
}

item list_last(list l)
{
	item p = l;
	while(p && p->next != NULL)
		p = p->next;
	return p;
}
int list_remove(list l,int idx)
{
	if(l==NULL) return 0;
	if(idx <0 || idx >= list_size(l)) return 0;
	if(idx == 0)
	{
		l=l->next;
		return 1;
	}
	item np = list_at(l,idx-1);
	if(np == NULL) return 0;
	if(np->next == NULL) return 1;
	np->next = np->next->next;
}
int list_size(list l)
{
	if(l == NULL || l->type == TYPE_NIL) return 0;
	int i=0;
	item np = l;
	while(np)
	{
		np = np->next;
		i++;
	}
	return i;
}
item list_at(list l,int idx)
{
	if(l == NULL || idx<0 || idx>= list_size(l)) 
		return NULL;
	int i=0;
	item np = l;
	while(np)
	{
		if(i==idx)
			return np;
		np = np->next;
		i++;
	}
	return NULL;
}
item new_list_item(int type, void* data)
{
	item ret = (item)malloc(sizeof *ret);
	ret->type = type;
    ret->value = data;
	ret->next = NULL;
	return ret;
}
void list_free(list *l)
{
	item curr;
	while ((curr = (*l)) != NULL) { 
	    (*l) = (*l)->next;
		if(curr->type == TYPE_LIST)
			list_free((list*)&curr->value);
        //if(curr->value) free(curr->value);
	    free (curr);
	}
}
int list_empty(list l)
{
	return l== NULL || l->type == TYPE_NIL;
}