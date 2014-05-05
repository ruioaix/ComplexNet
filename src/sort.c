#include "sort.h"

void qsort_i_desc(int s[], int l, int r)
{
	if (l < r)
	{
		int i = l, j = r, x = s[l];
		while (i < j)
		{
			while(i < j && s[j] < x)
				j--; 
			if(i < j)
				s[i++] = s[j];

			while(i < j && s[i] >= x)
				i++; 
			if(i < j)
				s[j--] = s[i];
		}
		s[i] = x;
		qsort_i_desc(s, l, i - 1);
		qsort_i_desc(s, i + 1, r);
	}
}

void qsort_i_asc(int s[], int l, int r)
{
	if (l < r)
	{
		int i = l, j = r, x = s[l];
		while (i < j)
		{
			while(i < j && s[j] >= x)
				j--; 
			if(i < j)
				s[i++] = s[j];

			while(i < j && s[i] < x)
				i++; 
			if(i < j)
				s[j--] = s[i];
		}
		s[i] = x;
		qsort_i_asc(s, l, i - 1);
		qsort_i_asc(s, i + 1, r);
	}
}

//l = 0, r = N-1, if array's length is N.
void qsort_ii_asc(int s[], int l, int r, int in[])
{
	if (l < r)
	{
		int i = l, j = r; 
		int x = s[l];
		int in_x=in[l];
		while (i < j)
		{
			while(i < j && s[j] >= x)
				j--; 
			if(i < j) {
				in[i]=in[j];
				s[i++] = s[j];
			}

			while(i < j && s[i] < x)
				i++; 
			if(i < j) {
				in[j]=in[i];
				s[j--] = s[i];
			}
		}
		s[i] = x;
		in[i]=in_x;
		qsort_ii_asc(s, l, i - 1, in);
		qsort_ii_asc(s, i + 1, r, in);
	}
}
void qsort_ii_desc(int s[], int l, int r, int in[])
{
	if (l < r)
	{
		int i = l, j = r; 
		int x = s[l];
		int in_x=in[l];
		while (i < j)
		{
			while(i < j && s[j] < x)
				j--; 
			if(i < j) {
				in[i]=in[j];
				s[i++] = s[j];
			}

			while(i < j && s[i] >= x)
				i++; 
			if(i < j) {
				in[j]=in[i];
				s[j--] = s[i];
			}
		}
		s[i] = x;
		in[i]=in_x;
		qsort_ii_desc(s, l, i - 1, in);
		qsort_ii_desc(s, i + 1, r, in);
	}
}

//l = 0, r = N-1, if array's length is N.
void qsort_id3_asc(int s[], int l, int r, double in[], double in1[], double in2[])
{
	if (l < r)
	{
		int i = l, j = r; 
		int x = s[l];
		double in_x=in[l];
		double in1_x=in1[l];
		double in2_x=in2[l];
		while (i < j)
		{
			while(i < j && s[j] >= x)
				j--; 
			if(i < j) {
				in[i]=in[j];
				in1[i]=in1[j];
				in2[i]=in2[j];
				s[i++] = s[j];
			}

			while(i < j && s[i] < x)
				i++; 
			if(i < j) {
				in[j]=in[i];
				in1[j]=in1[i];
				in2[j]=in2[i];
				s[j--] = s[i];
			}
		}
		s[i] = x;
		in[i]=in_x;
		in1[i]=in1_x;
		in2[i]=in2_x;
		qsort_id3_asc(s, l, i - 1, in, in1, in2);
		qsort_id3_asc(s, i + 1, r, in, in1, in2);
	}
}

void qsort_i3_asc(int s[], int l, int r, int in[], int in2[])
{
	if (l < r)
	{
		int i = l, j = r; 
		int x = s[l];
		int in_x=in[l];
		int in_y=in2[l];
		while (i < j)
		{
			while(i < j && s[j] >= x)
				j--; 
			if(i < j) {
				in[i]=in[j];
				in2[i]=in2[j];
				s[i++] = s[j];
			}

			while(i < j && s[i] < x)
				i++; 
			if(i < j) {
				in[j]=in[i];
				in2[j]=in2[i];
				s[j--] = s[i];
			}
		}
		s[i] = x;
		in[i]=in_x;
		in2[i]=in_y;
		qsort_i3_asc(s, l, i - 1, in, in2);
		qsort_i3_asc(s, i + 1, r, in, in2);
	}
}

void qsort_d_asc(double s[], int l, int r)
{
	if (l < r)
	{
		int i = l, j = r; 
		double x = s[l];
		while (i < j)
		{
			while(i < j && s[j] >= x)
				j--; 
			if(i < j) {
				s[i++] = s[j];
			}

			while(i < j && s[i] < x)
				i++; 
			if(i < j) {
				s[j--] = s[i];
			}
		}
		s[i] = x;
		qsort_d_asc(s, l, i - 1);
		qsort_d_asc(s, i + 1, r);
	}
}

void qsort_d_desc(double s[], int l, int r)
{
	if (l < r)
	{
		int i = l, j = r; 
		double x = s[l];
		while (i < j)
		{
			while(i < j && s[j] < x)
				j--; 
			if(i < j) {
				s[i++] = s[j];
			}

			while(i < j && s[i] >= x)
				i++; 
			if(i < j) {
				s[j--] = s[i];
			}
		}
		s[i] = x;
		qsort_d_desc(s, l, i - 1);
		qsort_d_desc(s, i + 1, r);
	}
}


void qsort_di_desc(double s[], int l, int r, int in[])
{
	if (l < r)
	{
		int i = l, j = r; 
		double x = s[l];
		int in_x=in[l];
		while (i < j)
		{
			while(i < j && s[j] < x)
				j--; 
			if(i < j) {
				in[i]=in[j];
				s[i++] = s[j];
			}

			while(i < j && s[i] >= x)
				i++; 
			if(i < j) {
				in[j]=in[i];
				s[j--] = s[i];
			}
		}
		s[i] = x;
		in[i]=in_x;
		qsort_di_desc(s, l, i - 1, in);
		qsort_di_desc(s, i + 1, r, in);
	}
}

void qsort_di_asc(double s[], int l, int r, int in[])
{
	if (l < r)
	{
		int i = l, j = r; 
		double x = s[l];
		int in_x=in[l];
		while (i < j)
		{
			while(i < j && s[j] >= x)
				j--; 
			if(i < j) {
				in[i]=in[j];
				s[i++] = s[j];
			}

			while(i < j && s[i] < x)
				i++; 
			if(i < j) {
				in[j]=in[i];
				s[j--] = s[i];
			}
		}
		s[i] = x;
		in[i]=in_x;
		qsort_di_asc(s, l, i - 1, in);
		qsort_di_asc(s, i + 1, r, in);
	}
}

void qsort_iid_asc(int s[], int l, int r, int in[], double in1[]) {
	if (l < r)
	{
		int i = l, j = r; 
		int x = s[l];
		int in_x=in[l];
		double in_y=in1[l];
		while (i < j)
		{
			while(i < j && s[j] >= x)
				j--; 
			if(i < j) {
				in[i]=in[j];
				in1[i]=in1[j];
				s[i++] = s[j];
			}

			while(i < j && s[i] < x)
				i++; 
			if(i < j) {
				in[j]=in[i];
				in1[j]=in1[i];
				s[j--] = s[i];
			}
		}
		s[i] = x;
		in[i]=in_x;
		in1[i]=in_y;
		qsort_iid_asc(s, l, i - 1, in, in1);
		qsort_iid_asc(s, i + 1, r, in, in1);
	}
}

//int binsearch_i_asc(int source[], int length, int key) {
//	int l=0, r=length-1, mid;
//	if(source[l] == key) {
//		return l;
//	}
//	if(source[r] == key) {
//		return r;
//	}
//	while(l<=r) {
//		mid = l + (r-l)/2;
//		if(source[mid] == key) {
//			return mid;
//		}
//		if(source[mid] > key) {
//			r=mid-1;
//		}
//		else {
//			l=mid+1;
//		}
//	}
//	return -1;
//}

void qsort_li_desc(long s[], int l, int r, int in[])
{
	if (l < r)
	{
		int i = l, j = r; 
		long x = s[l];
		int in_x=in[l];
		while (i < j)
		{
			while(i < j && s[j] < x)
				j--; 
			if(i < j) {
				in[i]=in[j];
				s[i++] = s[j];
			}

			while(i < j && s[i] >= x)
				i++; 
			if(i < j) {
				in[j]=in[i];
				s[j--] = s[i];
			}
		}
		s[i] = x;
		in[i]=in_x;
		qsort_li_desc(s, l, i - 1, in);
		qsort_li_desc(s, i + 1, r, in);
	}
}
