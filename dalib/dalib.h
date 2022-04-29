#ifndef DALIB_H
#define DALIB_H


#if	defined(_MSC_VER)
#	define _CRT_SECURE_NO_WARNINGS
#endif


#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <immintrin.h>



// string ////////////////////////////////////////////////////////////////////////////////////

#define	dalib_str_realloc(str, newsize)	\
	(str = realloc(str, sizeof(char) * newsize))

#define dalib_str_free(str)	\
	(str = dalib_str_realloc(str, sizeof str), free(str), str = NULL)

#define	dalib_str_alloc_assign(str, val)	\
	(str = malloc(dalib_str_length(val)), memcpy(str, val, sizeof val))

#define	dalib_str_copy(str, other)	\
	(	\
		dalib_str_realloc(str, dalib_str_length(other)),	\
		memcpy(str, other, dalib_str_length(other)),	\
		dalib_str_null_terminate(str, dalib_str_length(other))	\
	)

#define dalib_str_index_outofbound(str, idx)	\
	((idx < 0) | (idx > dalib_str_length(str) - 1))

#define	dalib_str_at(str, idx)	\
	(dalib_str_index_outofbound(str, idx) ? '\0' : str[idx])

#define dalib_str_length(str)	\
	(strlen(str))

#define dalib_str_resize_char(str, size, ch)	\
	{	\
		size_t len = (dalib_str_length(str) > size) ? size : dalib_str_length(str);	\
		dalib_str_realloc(str, size);	\
		memset(str + len, ch, dalib_str_length(str) - len);	\
	}

#define dalib_str_resize(str, size)	\
	dalib_str_resize_char(str, size, '\0')

#define dalib_str_front(str)	\
	(str[0])

#define dalib_str_back(str)	\
	(str[dalib_str_length(str) - 1])

#define	dalib_str_clear(str)	\
	(memset(str, '\0', dalib_str_length(str)))

#define	dalib_str_is_empty(str)	\
	(dalib_str_length(str) == 0)

#define	dalib_str_null_terminate(str, idx)	\
	(*(str + idx) = '\0')

bool dalib_str_insert_with_range
(char **str, size_t pos, const char *other, size_t range)
{
	if (!(*str) || !other)
		return false;

	pos = (pos > dalib_str_length((*str))) ? dalib_str_length((*str)) : pos;
	range = (range > dalib_str_length(other)) ? dalib_str_length(other) : range;

	size_t	lenstr = dalib_str_length((*str)),
		lenother = range,
		newsz = lenstr + lenother + 1;

	(*str) = dalib_str_realloc((*str), lenstr + lenother + 1);
	memmove((*str) + pos + lenother, (*str) + pos, lenstr - pos + 1);
	memcpy((*str) + pos, other, lenother);
	dalib_str_null_terminate((*str), lenstr + lenother);

	return true;
}

#define	dalib_str_insert(str, pos, other)	\
	(dalib_str_insert_with_range(&str, pos, other, dalib_str_length(other)))

#define dalib_str_append(str, other)	\
	(dalib_str_insert_with_range(&str, dalib_str_length(str), other, dalib_str_length(other)))

#define dalib_str_append_with_range(str, other, range)	\
	(dalib_str_insert_with_range(&str, dalib_str_length(str), other, range))

bool dalib_str_insert_other_from_pos_with_range
(char **str, size_t pos, const char *other, size_t other_pos, size_t range)
{
	if (!(*str) || !other)
		return false;

	pos = (pos > dalib_str_length((*str))) ? dalib_str_length((*str)) : pos;
	other_pos = (other_pos > dalib_str_length(other)) ? dalib_str_length(other) : other_pos;
	range = (range > dalib_str_length(other) - other_pos) ? dalib_str_length(other) - other_pos : range;

	size_t	lenstr = dalib_str_length((*str)),
		lenother = range;

	(*str) = dalib_str_realloc((*str), lenstr + lenother + 1);
	memmove((*str) + pos + lenother, (*str) + pos, lenstr - pos + 1);
	memcpy((*str) + pos, other + other_pos, lenother);
	dalib_str_null_terminate((*str), lenstr + lenother);

	return true;
}

#define	dalib_str_insert_other_from_pos(str, pos, other, other_pos)	\
	(dalib_str_insert_other_from_pos_with_range(&str, pos, other, other_pos, dalib_str_length(other) - other_pos))

bool dalib_str_erase_with_range
(char **str, size_t pos, size_t range)
{
	if (!(*str))	return false;

	pos = (pos > dalib_str_length((*str))) ? dalib_str_length((*str)) : pos;
	range = (range > dalib_str_length((*str)) - pos) ? dalib_str_length((*str)) - pos : range;

	size_t	lenremain = dalib_str_length((*str)) - pos - range,
		posremain = pos + range;
	char ch = (*str)[posremain];

	memmove((*str) + pos, (*str) + posremain, lenremain);
	dalib_str_realloc((*str), pos + lenremain + 1);
	dalib_str_null_terminate((*str), pos + lenremain);

	return true;
}

#define	dalib_str_erase(str, pos)	\
	(dalib_str_erase_with_range(&str, pos, dalib_str_length(str) - pos))

bool dalib_str_replace_char_range
(char *str, size_t pos, char ch, size_t range)
{
	if (!str)	return false;

	pos = (pos > dalib_str_length(str)) ? dalib_str_length(str) : pos;
	range = (range > dalib_str_length(str) - pos) ? dalib_str_length(str) - pos : range;
	memset(str + pos, ch, range);

	return true;
}

#define	dalib_str_replace_char(str, pos, ch)	\
	(dalib_str_replace_char_range(str, pos, ch, sizeof char))

bool dalib_str_replace_substr_range
(char *str, size_t pos, const char *other, size_t range)
{
	if (!str)	return false;

	pos = (pos > dalib_str_length(str)) ? dalib_str_length(str) : pos;
	range = (range > dalib_str_length(other)) ? dalib_str_length(other) : range;

	memcpy(str + pos, other, range);
	return true;
}

#define	dalib_str_replace_substr(str, pos, other)	\
	(dalib_str_replace_substr_range(str, pos, other, dalib_str_length(other)))

bool dalib_str_swap
(char **str1, char **str2)
{
	if (!*str1 || !*str2)
		return false;

	char *temp = *str1;
	*str1 = *str2;
	*str2 = temp;

	return true;
}

bool dalib_str_push_back
(char *str, char ch)
{
	if (!str)	return false;

	size_t	lenstr = dalib_str_length(str);

	dalib_str_realloc(str, lenstr + 1);
	memset(str + lenstr, ch, sizeof(char));
	dalib_str_null_terminate(str, lenstr + 1);

	return true;
}

bool dalib_str_pop_back
(char *str)
{
	if (!str)	return false;

	size_t	lenstr = dalib_str_length(str);

	str = realloc(str, lenstr - 1);
	dalib_str_null_terminate(str, lenstr - 1);

	return true;
}

#define	dalib_str_equal(str1, str2)	\
	((!str1 && !str2) | (!strcmp(str1, str2)))

const char *dalib_str_substr_range
(const char *str, size_t pos, size_t range)
{
	if (!str || pos > dalib_str_length(str) || range > dalib_str_length(str))
		return NULL;

#if	defined(_MSC_VER)
	char *substr = malloc(range);
#elif
	const size_t lensubstr = range;
	char substr[lensubstr];
#endif

	memcpy(substr, str + pos, range);
	dalib_str_null_terminate(substr, range);

	return substr;
}

#define	dalib_str_substr(str, pos)	\
	(dalib_str_substr_range(str, pos, dalib_str_length(str) - pos))

bool dalib_str_reverse
(char *str)
{
	if (!str)	return false;

	const __m128i	_mask = _mm_set_epi8(
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
	);

	int	lenstr = dalib_str_length(str);
	char	*front = &str[0],
		*back = &str[dalib_str_length(str) - 1];

	for (; lenstr >= 16; front += 16, back -= 16, lenstr -= 32) {
		__m128i	_vfront = _mm_loadu_si128((__m128i *)front),
			_vback = _mm_loadu_si128((__m128i *)(back - 15));

		_vfront = _mm_shuffle_epi8(_vfront, _mask);
		_vback = _mm_shuffle_epi8(_vback, _mask);

		_mm_storeu_si128((__m128i *)(back - 15), _vfront);
		_mm_storeu_si128((__m128i *)(front), _vback);
	}

	for (; front < back; ++front, --back) {
		const char temp = *front;
		*front = *back;
		*back = temp;
	}

	return true;
}

int dalib_str_find_first
(const char *str, const char *substr)
{
	if (!str || !substr || !strstr(str, substr))
		return -1;

	return (int)(strstr(str, substr) - str);
}

int dalib_str_find_last
(const char *str, const char *substr)
{
	if (!str || !substr || !strstr(str, substr))
		return -1;

	int		foundidx = -1;
	size_t		lensubstr = dalib_str_length(substr);
	const char	*ptr = str;
	for (; ptr = strstr(ptr, substr); foundidx = ptr - str, ptr += lensubstr);
	return foundidx;
}

bool dalib_str_remove_whitespace
(char *str) 
{
	if (!str)	return false;

	char	*w = str,
		*r = str;

	do
		if (*r != ' ')
			*w++ = *r;
	while (*r++);

	return true;
}


// stack /////////////////////////////////////////////////////////////////////////////////////

typedef struct s_dalib_stack_type {

	void		*_ptr;
	const char	*_t_nm;

	size_t		_elem_sz;
	size_t		_sz;
	size_t		_cap;

}
dalib_stack_t;


dalib_stack_t dalib_stack_new
(const char *element_type, const size_t element_size, const size_t capacity)
{
	dalib_stack_t newstack;
	newstack._ptr = malloc(element_size * capacity);
	newstack._t_nm = element_type;
	newstack._elem_sz = element_size;
	newstack._sz = 0;
	newstack._cap = capacity;

	return newstack;
}

#define dalib_stack_new(type, capacity)	\
	(dalib_stack_new(#type, sizeof(type), capacity))

dalib_stack_t dalib_stack_assign
(void **stack_ptr, const char *element_type, const size_t element_size, const size_t capacity)
{
	if (!(*stack_ptr))
		*stack_ptr = malloc(element_size * capacity);
	else {
		void *newptr = realloc(*stack_ptr, element_size * capacity);
		*stack_ptr = (!newptr) ? *stack_ptr : newptr;
	}

	dalib_stack_t newstack;
	newstack._ptr = *stack_ptr;
	newstack._t_nm = element_type;
	newstack._elem_sz = element_size;
	newstack._sz = 0;
	newstack._cap = capacity;

	return newstack;
}

#define dalib_stack_assign(stack_ptr, type, capacity)	\
	(dalib_stack_assign(&stack_ptr, #type, sizeof(type), capacity))

#define	dalib_stack_free(stack)	\
	((stack)->_t_nm = NULL, free((stack)->_ptr))

#define	dalib_stack_get_data(stack, type)	\
	((type *)(stack)->_ptr)

#define dalib_stack_get_type(stack)	\
	((stack)->_t_nm)

#define dalib_stack_get_element_size(stack)	\
	((stack)->_elem_sz)

#define dalib_stack_get_size(stack)	\
	((stack)->_sz)

#define	dalib_stack_get_capacity(stack)	\
	((stack)->_cap)

bool dalib_stack_grow
(dalib_stack_t *stack)
{
	if (!stack)	return false;

	stack->_cap = (stack->_cap * 2) + ((stack->_cap == 0) ? 1 : 0);

	void *newptr = realloc(stack->_ptr, stack->_elem_sz * stack->_cap);
	stack->_ptr = (!newptr) ? stack->_ptr : newptr;

	return true;
}

#define	dalib_stack_grow(stack)	\
	(dalib_stack_grow(stack))

#define	dalib_stack_is_empty(stack)	\
	((!(stack)) | ((stack)->_sz == 0))

#define	dalib_stack_push_back(stack, type, value)	\
	{	\
		if ((((stack) != NULL) && dalib_str_equal((stack)->_t_nm, #type))) {	\
			if ((stack)->_sz >= (stack)->_cap) {	\
				dalib_stack_grow((stack));	\
			}	\
				\
			type newval = value;	\
			*((type*)(stack)->_ptr + (stack)->_sz++) = newval;	\
		}	\
	}

#define dalib_stack_pop_back(stack)	\
	{	\
		if (((stack) != NULL) && !dalib_stack_is_empty(stack)) {	\
			(stack)->_sz--;	\
		}	\
	}

#define	dalib_stack_at(stack, type, pos)	\
	(	\
		(((stack) != NULL) && dalib_str_equal((stack)->_t_nm, #type) && !dalib_stack_is_empty((stack)))	\
		?	*((type *)(stack)->_ptr + ((pos < 0 ? 0 : pos >= dalib_stack_get_size(stack) ? dalib_stack_get_size(stack) - 1 : pos)))	\
		:	(type){0}	\
	)

#define	dalib_stack_peek(stack, type)	\
	(dalib_stack_at(stack, type, dalib_stack_get_size(stack) - 1))

#define	dalib_stack_reverse(stack, type)	\
	{	\
		if (((stack) != NULL) && !dalib_stack_is_empty(stack) && dalib_str_equal((stack)->_t_nm, #type)) {	\
			type	*front = (type *)(stack)->_ptr;	\
			type	*back = (type *)(stack)->_ptr + (stack)->_sz - 1;	\
				\
			for (; front < back; ++front, --back) {	\
				type temp = *front;	\
				*front = *back;	\
				*back = temp;	\
			}	\
		}	\
	}

#define	dalib_stack_get_front(stack, type)	\
	((type *)(stack)->_ptr)

#define	dalib_stack_get_back(stack, type)	\
	((type *)(stack)->_ptr + dalib_stack_get_size(stack) - 1)



#endif
