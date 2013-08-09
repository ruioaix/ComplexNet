#include "../inc/crosslist.h"

struct crosslist *createCrossList(int rowLen, int colLen)
{
	struct crosslist *cl=calloc(1, sizeof(struct crosslist));
	memError(cl, "createCrossList cl");

	cl->row_max=rowLen;
	cl->col_max=colLen;

	cl->row=calloc(rowLen, sizeof(struct cl_element));
	memError(cl->row, "createCrossList cl->row");

	cl->col=calloc(colLen, sizeof(struct cl_element));
	memError(cl->col, "createCrossList cl->col");

	return cl;
}

void insertElementCL(struct crosslist *cl, struct cl_element *cl_em)
{
	int x=cl_em->x;
	int y=cl_em->y;
	if (x>=cl->row_max || y>=cl->col_max) {
		isError("insertElementCL: the element is wrong");
	}

	struct cl_element *pos, *last;

	// insert into row;
	pos=cl->row[x];
	last=cl->row[x];
	while(pos) {
		if (pos->y==y) {
			isError("insertElementCL: already exist the element.");
		} 
		else if(pos->y<y) {
			last=pos;
			pos=pos->right;
			continue;
		}
		else if(pos->y>y) {
			if (pos->left==pos) {
				cl->row[x]=cl_em;
				//the first's left points to itself.
				cl_em->left=cl_em;
				cl_em->right=pos;
				pos->left=cl_em;
			} else {
				cl_em->left=pos->left;
				cl_em->right=pos;
				pos->left->right=cl_em;
				pos->left=cl_em;
			}
			break;
		}
	}
	if (!pos) {
		if (pos==cl->row[x]) {
			cl->row[x]=cl_em;
			//the first's left points to itself.
			cl_em->left=cl_em;
			cl_em->right=NULL;
		} else {
			last->right=cl_em;
			cl_em->left=last;
			cl_em->right=NULL;	
		}
	}

	// insert into col;
	pos=cl->col[y];
	last=cl->col[y];
	while(pos) {
		if (pos->x==x) {
			isError("insertElementCL: already exist the element.");
		} 
		else if(pos->x<x) {
			last=pos;
			pos=pos->down;
			continue;
		}
		else if(pos->x>x) {
			if (pos->up==pos) {
				cl->col[y]=cl_em;
				//the first's left points to itself.
				cl_em->up=cl->col[y];
				cl_em->down=pos;
				pos->up=cl_em;
			} else {
				cl_em->up=pos->up;
				cl_em->down=pos;
				pos->up->down=cl_em;
				pos->up=cl_em;
			}
			break;
		}
	}
	if (!pos) {
		if (cl->col[y]==pos) {
			cl->col[y]=cl_em;
			//the first's left points to itself.
			cl_em->up=cl_em;
			cl_em->down=NULL;
		} else {
			last->down=cl_em;
			cl_em->up=last;
			cl_em->down=NULL;	
		}
	}
}

//if the element doesn't exist, nothig been done;
void deleteElementCL(struct crosslist *cl, int x, int y)
{
	struct cl_element *temp;
	temp=cl->row[x];
	while(temp) {
		if (temp->y==y) {
			if (temp->left == temp) {
				cl->row[x]=temp->right;
				if (temp->right) {
					temp->right->left=cl->row[x];
				}
			} else {
				temp->left->right=temp->right;
				if (temp->right) {
					temp->right->left=temp->left;
				}
			}
			if (temp->up == temp) {
				cl->col[y]=temp->down;
				if (temp->down) {
					temp->down->up=cl->col[y];
				}
			} else {
				temp->up->down=temp->down;
				if (temp->down) {
					temp->down->up=temp->up;
				}
			}
			free_cl_element(temp);
			break;
		}
		else if (temp->y<y) {
			temp=temp->right;
		}
		else {
			break;
		}
	}
}

struct cl_element *getElementCL(struct crosslist *cl, int x, int y)
{
	struct cl_element *pos;
	pos=cl->row[x];
	while(pos) {
		if(pos->y==y) {
			return pos;
		}
		else if(pos->y<y) {
			pos=pos->right;
		}
		else {
			break;
		}
	}
	return NULL;	
}

void free_cl_element(struct cl_element *cl_em)
{
	free(cl_em->time);
	free(cl_em);
}

void free_crosslist(struct crosslist *cl)
{
	struct cl_element *pos, *temp;
	int i;
	for (i=0; i<cl->row_max; ++i) {
		pos=cl->row[i];
		while(pos) {
			temp=pos->right;	
			free_cl_element(pos);
			pos=temp;
		}
	}
	free(cl->row);
	free(cl->col);
	free(cl);
}
