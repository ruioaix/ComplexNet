#include "../inc/complexnet_sort.h"

void quick_sort_int(int s[], int l, int r)
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
		quick_sort_int(s, l, i - 1);
		quick_sort_int(s, i + 1, r);
	}
}

//l = 0, r = N-1, if array's length is N.
void quick_sort_int_index(int s[], int l, int r, int in[])
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
		quick_sort_int_index(s, l, i - 1, in);
		quick_sort_int_index(s, i + 1, r, in);
	}
}

//l = 0, r = N-1, if array's length is N.
void quick_sort_int_index3(int s[], int l, int r, double in[], double in1[], double in2[])
{
	if (l < r)
	{
		int i = l, j = r; 
		int x = s[l];
		int in_x=in[l];
		int in1_x=in1[l];
		int in2_x=in2[l];
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
		quick_sort_int_index3(s, l, i - 1, in, in1, in2);
		quick_sort_int_index3(s, i + 1, r, in, in1, in2);
	}
}

void quick_sort_int_index_index(int s[], int l, int r, int in[], int in2[])
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
		quick_sort_int_index_index(s, l, i - 1, in, in2);
		quick_sort_int_index_index(s, i + 1, r, in, in2);
	}
}

void quick_sort_double(double s[], int l, int r)
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
		quick_sort_double(s, l, i - 1);
		quick_sort_double(s, i + 1, r);
	}
}

void quick_sort_double_index(double s[], int l, int r, int in[])
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
		quick_sort_double_index(s, l, i - 1, in);
		quick_sort_double_index(s, i + 1, r, in);
	}
}
