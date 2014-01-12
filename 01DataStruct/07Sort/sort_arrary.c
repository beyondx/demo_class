#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct  data_info {
	char *name;
	int  age;
};
#define   _NtoPTR(base, n, size)		\
		((char *)base + ((n) * size))
#define   NtoPTR(n)	(_NtoPTR(base, n, size))

void swap(void *a, void *b, size_t size)
{
	char *tmp = (char *)malloc(size);
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
#ifdef DEBUG
	static int count = 0;
	printf("count = %d\n", ++count);
#endif
	free(tmp);
}

void  bubble_sort(void *base, size_t nmemb, size_t size, 
		int (*cmp)(const void *, const void *))
{
	assert(base != NULL);
	assert(cmp  != NULL);

	int start, end;
	for (end = nmemb - 1; end >= 1; --end) {
		for (start = 0; start < end; ++start) {
			if (cmp(NtoPTR(start), NtoPTR(start + 1)) > 0) {
				swap(NtoPTR(start), NtoPTR(start + 1), size);	
			} 
		}
	}
}


void  select_sort(void *base, size_t nmemb, size_t size, 
		int (*cmp)(const void *, const void *))
{
	assert(base != NULL);
	assert(cmp != NULL);

	size_t i, j, min;

	for (i = 0; i < nmemb - 1; ++i) {
		min = i;
		for (j = i + 1; j < nmemb; ++j) {
			if (cmp(NtoPTR(j), NtoPTR(min)) < 0) {
				min = j;
			}
		}
		if (min  != i) {
			swap(NtoPTR(i), NtoPTR(min), size);
		}
	}
}

/* 1.插入排序比较适合于：数据比较小， 数据部分有序*/
void  insert_sort(void *base, size_t nmemb, size_t size, 
		int (*cmp)(const void *, const void *))
{
	assert(base != NULL);
	assert(cmp != NULL);

	char *buf = (char *)malloc(size);
	assert(buf != NULL);
	memset(buf, 0, size);
	
	int i,j;
	for (i = 1; i < nmemb; ++i) {
		j = i - 1;
		if (cmp(NtoPTR(i), NtoPTR(j)) >= 0) {
			continue;
		} else {
			memcpy(buf, NtoPTR(i), size);
		}  
	
		for (; j >= 0; --j) {
			if (cmp(NtoPTR(j), buf) > 0) {
				memcpy(NtoPTR(j + 1), NtoPTR(j), size);
			} else {
				break;
			}	
		}
		
		memcpy(NtoPTR(j + 1), buf, size);
	}
	
}



void shell_sort(void *base, size_t nmemb, size_t size, 
		int (*cmp)(const void *, const void *))
{
	size_t step[] = {3, 2, 1};
	
	int i, j, k, l, offset;

	char *buf = (char *)malloc(size);
	assert(buf != NULL);
	memset(buf, 0, size);	


	for (i = 0; i < sizeof(step) / sizeof(step[0]); ++i) {
#ifdef DEBUG
{
	printf("step[%lu]:\n", step[i]);
}
#endif
		for ( offset = 0; offset < step[i]; ++offset) {
#ifdef DEBUG
{
	for (j = offset; j < nmemb; j += step[i]) {
		struct data_info *pa = (struct data_info *)NtoPTR(j);
		printf("%d ", pa->age);
	}
	printf("\n-----------sorting---\n");
}
#endif
//TODO  insert_sort
	for (j = offset + step[i]; j < nmemb; j += step[i]) {
		k = j - step[i];
		if (cmp(NtoPTR(j), NtoPTR(k)) >= 0) {
			continue;
		}
		memcpy(buf, NtoPTR(j), size);
		for (; k >= offset; k -= step[i]) {
			if (cmp(NtoPTR(k), buf) > 0) {
				memcpy(NtoPTR(k + step[i]), NtoPTR(k), size);
			} else {
				break;
			}
		}
		memcpy(NtoPTR(k + step[i]), buf, size);
	}	

#ifdef DEBUG
{
	for (j = offset; j < nmemb; j += step[i]) {
		struct data_info *pa = (struct data_info *)NtoPTR(j);
		printf("%d ", pa->age);
	}
	printf("\n=====================\n");
}
#endif
//TODO  insert_sort
		}	
	}

	free(buf);	
	
}

void __merge_sort(void *base, size_t nmemb, size_t size, void *buf,
		int (*cmp)(const void *, const void *))
{
	/*拆分*/
	if (nmemb <= 1) {
		return ;
	} else {
		__merge_sort(base, nmemb / 2, size, buf, cmp);
		__merge_sort(NtoPTR(nmemb / 2), nmemb - nmemb / 2, size, buf, cmp);
	}
	/*有序合并*/
	size_t l, r, i;
	l = 0;
	r = nmemb / 2;
	i = 0;

	while ((l < nmemb / 2) && (r < nmemb)) {
		if (cmp(NtoPTR(l), NtoPTR(r)) < 0) {
			memcpy(_NtoPTR(buf, i, size), NtoPTR(l), size);
			++l;
		} else {
			memcpy(_NtoPTR(buf, i, size), NtoPTR(r), size);
			++r;
		}
		++i;
	}
	
	/*未完全的数据写入缓冲区*/
	if (l < nmemb / 2) {
		memcpy(_NtoPTR(buf, i, size), NtoPTR(l), size * (nmemb / 2 - l));
	} else {
		memcpy(_NtoPTR(buf, i, size), NtoPTR(r), size * (nmemb - r));
	}
#ifdef DEBUG
{
	struct data_info *p = buf;
	printf("merge_sort debug\n");
	for (i = 0; i < nmemb; ++i) {
		printf("[Debug]:%s:%d\n",p[i].name, p[i].age);
	}
}
#endif
	/*回写到原有空间中*/
	memcpy(base, buf, size * nmemb);
}

void merge_sort(void *base, size_t nmemb, size_t size, 
		int (*cmp)(const void *, const void *))
{
	char *buf = (char *)calloc(nmemb, size);
	assert(buf != NULL);
	
	__merge_sort(base, nmemb, size, buf, cmp);
	free(buf);
}

void __quick_sort(void *base, size_t nmemb, size_t size, void *buf,
		int (*cmp)(const void *, const void *))
{
	if (nmemb < 2) {
		return ;
	}

	size_t  l, r, pivot;

	pivot = 0;
	l = 0;
	r = nmemb - 1;

	memcpy(buf, NtoPTR(pivot), size);
	
	for (; l < r; ) {
		//自右相左找出比轴值更小的元素
		for (; l < r; --r) {
			if (cmp(NtoPTR(r), buf) < 0) {
				memcpy(NtoPTR(l), NtoPTR(r), size);
				++l;
				break;
			}
		}
		//自左向右找出比轴值更大的元素
		for (; l < r; ++l) {
			if (cmp(NtoPTR(l), buf) > 0) {
				memcpy(NtoPTR(r), NtoPTR(l), size);
				--r;
				break;
			}
		}
	}

	// l == r相遇
	memcpy(NtoPTR(l), buf, size);

	//尾巴递归 TODO
	__quick_sort(base, l, size, buf, cmp); //left
	__quick_sort(NtoPTR(l + 1), nmemb - l - 1, size, buf, cmp);	
}

void quick_sort(void *base, size_t nmemb, size_t size, 
		int (*cmp)(const void *, const void *))
{
	char *buf = (char *)calloc(1, size);
	assert(buf != NULL);

	__quick_sort(base, nmemb, size, buf, cmp);
	free(buf);
}

int cmp_age(const void *a, const void *b)
{
	const struct data_info *pa = (const struct data_info *)a;
	const struct data_info *pb = (const struct data_info *)b;
	return pa->age - pb->age;
}

int main()
{
	struct data_info s[] = {
		{"张三丰", 3},
		{"张翠山", 2},
		{"张无忌", 7},
		{"殷素素", 7},
		{"赵敏", 4},
		{"周芷若", 5},
		{"谢逊", 1},
		{"杨逍", 6},
	};

	//qsort(s, sizeof s/ sizeof s[0], sizeof(s[0]), cmp_age);

	void (*p_sort[])(void *base, size_t nmemb, size_t size, 
		int (*cmp)(const void *, const void *)) = {
		qsort,
		bubble_sort,
		select_sort,
		insert_sort,
		shell_sort,
		merge_sort,
		quick_sort,
	};
	
	p_sort[6](s, sizeof s/ sizeof s[0], sizeof(s[0]), cmp_age);

	int i;
	for (i = 0; i < sizeof s/ sizeof s[0]; ++i) {
		printf("%s:%d\n", s[i].name, s[i].age);
	}
	
	return 0;
}
