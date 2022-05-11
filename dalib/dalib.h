#ifndef DALIB_H
#define DALIB_H


#if	defined(_MSC_VER)
#	define	_CRT_SECURE_NO_WARNINGS
#endif

#if	!defined(bool)
#define	bool	unsigned char
#define	true	1
#define	false	0
#endif


#include <stdlib.h>
#include <string.h>
#include <immintrin.h>


// string ////////////////////////////////////////////////////////////////////////////////////////////

// -- helper == //

#define	_dalib_helper_str_access_out_of_bound(str, index)	\
	((index < 0) | (index > strlen(str) - 1))

#define	_dalib_helper_str_is_empty(str)	\
	(strlen(str) == 0)

bool _dalib_helper_str_insert_with_range
(char **str, size_t pos, const char *other, size_t range)
{
	if (!(*str) || _dalib_helper_str_is_empty((*str)))
		return false;

	if (!other || _dalib_helper_str_is_empty(other))
		return false;

	pos = (pos > strlen((*str))) ? strlen((*str)) : pos;
	range = (range > strlen(other)) ? strlen(other) : range;

	size_t	lenstr = strlen((*str)),
		lenother = range,
		newsz = lenstr + lenother + 1;

	(*str) = realloc((*str), lenstr + lenother + 1);
	memmove((*str) + pos + lenother, (*str) + pos, lenstr - pos + 1);
	memcpy((*str) + pos, other, lenother);
	*((*str) + lenstr + lenother) = '\0';

	return true;
}

bool _dalib_helper_str_insert_from_pos_with_range
(char **str, size_t pos, const char *other, size_t other_pos, size_t range)
{
	if (!(*str) || _dalib_helper_str_is_empty((*str)))
		return false;

	if (!other || _dalib_helper_str_is_empty(other))
		return false;

	pos = (pos > strlen((*str))) ? strlen((*str)) : pos;
	other_pos = (other_pos > strlen(other)) ? strlen(other) : other_pos;
	range = (range > strlen(other) - other_pos) ? strlen(other) - other_pos : range;

	size_t	lenstr = strlen((*str)),
		lenother = range;

	(*str) = realloc((*str), lenstr + lenother + 1);
	memmove((*str) + pos + lenother, (*str) + pos, lenstr - pos + 1);
	memcpy((*str) + pos, other + other_pos, lenother);
	*((*str) + lenstr + lenother) = '\0';

	return true;
}

bool _dalib_helper_str_erase_with_range
(char **str, size_t pos, size_t range)
{
	if (!(*str))	return false;

	pos = (pos > strlen((*str))) ? strlen((*str)) : pos;
	range = (range > strlen((*str)) - pos) ? strlen((*str)) - pos : range;

	size_t	lenremain = strlen((*str)) - pos - range,
		posremain = pos + range;
	char ch = (*str)[posremain];

	memmove((*str) + pos, (*str) + posremain, lenremain);
	realloc((*str), pos + lenremain + 1);
	*((*str) + pos + lenremain) = '\0';

	return true;
}

bool _dalib_helper_str_replace_char_with_range
(char **str, size_t pos, char ch, size_t range)
{
	if (!(*str))	return false;

	pos = (pos > strlen((*str))) ? strlen((*str)) : pos;
	range = (range > strlen((*str)) - pos) ? strlen(*(str)) - pos : range;
	memset((*str) + pos, ch, range);

	return true;
}

bool _dalib_helper_str_replace_substr_with_range
(char **str, size_t pos, const char *other, size_t range)
{
	if (!(*str))	return false;

	pos = (pos > strlen((*str))) ? strlen((*str)) : pos;
	range = (range > strlen(other)) ? strlen(other) : range;

	memcpy((*str) + pos, other, range);
	return true;
}

bool _dalib_helper_str_reverse
(char **str)
{
	if (!(*str))	return false;

	const __m128i	_mask = _mm_set_epi8(
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
	);

	size_t	lenstr = strlen((*str));
	char	*front = &(*str)[0],
		*back = &(*str)[strlen((*str)) - 1];

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

int _dalib_helper_str_find_first
(const char *str, const char *substr)
{
	if (!str || !substr || !strstr(str, substr))
		return -1;

	return (int)(strstr(str, substr) - str);
}

int _dalib_helper_str_find_last
(const char *str, const char *substr)
{
	if (!str || !substr || !strstr(str, substr))
		return -1;

	int		foundidx = -1;
	size_t		lensubstr = strlen(substr);
	const char	*ptr = str;
	for (; ptr = strstr(ptr, substr); foundidx = ptr - str, ptr += lensubstr);
	return foundidx;
}

const char *_dalib_helper_str_substr_range
(const char *str, size_t pos, size_t range)
{
	if (!str || pos > strlen(str) || range > strlen(str))
		return NULL;

#if	defined(_MSC_VER)
	char *substr = malloc(range);
#elif
	const size_t lensubstr = range;
	char substr[lensubstr];
#endif

	memcpy(substr, str + pos, range);
	*(substr + range) = '\0';

	return substr;
}

bool _dalib_helper_str_remove_whitespace
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


// -- public -- //

#define	dalib_str_repopulate(str, value)	\
	{	\
		if (!str) {	\
			str = malloc(sizeof(value));	\
			memcpy(str, value, sizeof(value));	\
		}	\
		else {	\
			str = realloc(str, sizeof(value));	\
			memcpy(str, value, sizeof(value));	\
		}	\
	}

#define	dalib_str_destroy(str)	\
	((!str) ? false : (str = realloc(str, sizeof(char)), free(str)), str = NULL)

#define	dalib_str_get_length(str)	\
	((!str) ? false : strlen(str))

#define	dalib_str_at_index(str, index)	\
	(_dalib_helper_str_access_out_of_bound(str, index) ? '\0', str[index])

#define	dalib_str_null_terminate(str, index)	\
	(_dalib_helper_str_access_out_of_bound(str, index) ? false : (*(str[index] = '\0')))

#define	dalib_str_get_front(str)	\
	(_dalib_helper_str_is_empty(str) ? '\0' : str[0])

#define	dalib_str_get_back(str)	\
	(_dalib_helper_str_is_empty(str) ? '\0' : str[strlen(str) - 1])

#define	dalib_str_clear(str)	\
	((!str) ? false : memset(str, 0, strlen(str))) 

#define	dalib_str_clear_keep_length(str)	\
	((!str) ? false : memset(str, '_', strlen(str))) 

#define	dalib_str_is_empty(str)	\
	(_dalib_helper_str_is_empty(str))

#define	dalib_str_insert(str, pos, other)	\
	(_dalib_helper_str_insert_with_range(&str, pos, other, strlen(other)))

#define	dalib_str_insert_range(str, pos, other, range)	\
	(_dalib_helper_str_insert_with_range(&str, pos, other, range))

#define	dalib_str_append(str, other)	\
	(_dalib_helper_str_insert_with_range(&str, strlen(str), other, strlen(other)))

#define	dalib_str_append_range(str, other, range)	\
	(_dalib_helper_str_insert_with_range(&str, strlen(str), other, range))

#define	dalib_str_insert_from_pos(str, pos, other, other_pos)	\
	(_dalib_helper_str_insert_from_pos_with_range(&str, pos, other, other_pos, strlen(other) - other_pos))

#define	dalib_str_insert_from_pos_with_range(str, pos, other, other_pos, range)	\
	(_dalib_helper_str_insert_from_pos_with_range(&str, pos, other, other_pos, range))

#define	dalib_str_erase(str, pos)	\
	(_dalib_helper_str_erase_with_range(&str, pos, strlen(str) - pos))

#define	dalib_str_erase_range(str, pos, range)	\
	(_dalib_helper_str_erase_with_range(&str, pos, range))

#define	dalib_str_replace_char(str, pos, ch)	\
	(_dalib_helper_str_replace_char_with_range(&str, pos, ch, strlen(str) - pos))

#define	dalib_str_replace_char_range(str, pos, ch, range)	\
	(_dalib_helper_str_replace_char_with_range(&str, pos, ch, range))

#define	dalib_str_replace_substring(str, pos, substr)	\
	(_dalib_helper_str_replace_substr_with_range(&str, pos, substr, strlen(substr)))

#define	dalib_str_replace_substring_range(str, pos, substr, range)	\
	(_dalib_helper_str_replace_substr_with_range(&str, pos, substr, range))

#define	dalib_str_swap(str, other_str)	\
	{	\
		if (str && other_str) {	\
			char *temp = str;	\
			str = other_str;	\
			other_str = temp;	\
		}	\
	}

#define	dalib_str_push_back(str, ch)	\
	{	\
		if (str) {	\
			size_t lenstr = strlen(str);	\
			str = realloc(str, lenstr + 1);	\
			memset(str + lenstr, ch, sizeof(char));	\
			*(str + lenstr + 1) = '\0';	\
		}	\
	}

#define	dalib_str_pop_back(str)	\
	{	\
		if (str) {	\
			size_t lenstr = strlen(str);	\
			str = realloc(str, lenstr - 1);	\
			*(str + lenstr - 1) = '\0';	\
		}	\
	}

#define	dalib_str_equal(str, other_str)	\
	((!str && !other_str) | (!strcmp(str, other_str)))

#define	dalib_str_substr(str, pos)	\
	(_dalib_helper_str_substr_range(str, pos, strlen(str) - pos))

#define	dalib_str_substr_range(str, pos, range)	\
	(_dalib_helper_str_substr_range(str, pos, range))

#define	dalib_str_reverse(str)	\
	(_dalib_helper_str_reverse(&str))

#define	dalib_str_find_first(str, substr)	\
	(_dalib_helper_str_find_first(str, substr))

#define	dalib_str_find_last(str, substr)	\
	(_dalib_helper_str_find_last(str, substr))

#define	dalib_str_remove_whitespace(str)	\
	(_dalib_helper_str_remove_whitespace(str))



// stack /////////////////////////////////////////////////////////////////////////////////////////////

typedef struct s_dalib_stack_header_type {

	char		*type_name;

	size_t		elem_size;
	size_t		length;
	size_t		capacity;

}
dalib_stack_header_t;


// -- helper -- //

const int _dalib_helper_stack_header_length = 4;
const int _dalib_helper_stack_growth_factor = 2;

void *_dalib_helper_stack_new
(const size_t elem_size, const char *type_name, const size_t capacity)
{
	size_t		headersz = sizeof(dalib_stack_header_t);
	size_t		stacksz = elem_size * capacity;

	intptr_t	*newstack = malloc(headersz + stacksz);
	memset(newstack, 0, headersz + stacksz);

	dalib_stack_header_t *header = (dalib_stack_header_t *)newstack;
	header->type_name = malloc(strlen(type_name));
	memcpy(header->type_name, type_name, strlen(type_name));
	header->elem_size = elem_size;
	header->length = 0;
	header->capacity = capacity;

	return (void *)(newstack + _dalib_helper_stack_header_length);
}

bool _dalib_helper_stack_free
(void **stack)
{
	if (!(*stack))	return false;

	dalib_stack_header_t *header = (dalib_stack_header_t *)((intptr_t *)(*stack) - _dalib_helper_stack_header_length);
	free(header);

	return true;
}

void *_dalib_helper_stack_grow
(void *stack)
{
	if (!stack)	return false;

	dalib_stack_header_t	*header = (dalib_stack_header_t *)((intptr_t *)stack - _dalib_helper_stack_header_length);
	header->capacity *= _dalib_helper_stack_growth_factor;
	header->capacity += (header->capacity == 0);

	void *newstack = _dalib_helper_stack_new(header->elem_size, header->type_name, header->capacity);
	memcpy(newstack, stack, header->elem_size * header->capacity);
	
	dalib_stack_header_t	*newheader = (dalib_stack_header_t *)((intptr_t *)newstack - _dalib_helper_stack_header_length);
	newheader->length = header->length;

	free(header);
	return newstack;
}

dalib_stack_header_t *_dalib_helper_stack_get_header
(void *stack)
{
	if (!stack)	
		return NULL;
	else
		return (dalib_stack_header_t *)((intptr_t *)stack - _dalib_helper_stack_header_length);
}

bool _dalib_helper_stack_is_empty
(void **stack) 
{
	if (!(*stack))	return false;

	dalib_stack_header_t *header = (dalib_stack_header_t *)((intptr_t *)(*stack) - _dalib_helper_stack_header_length);
	return (header->length == 0);
}


// -- public -- //

#define	dalib_stack_new(type)	\
	(_dalib_helper_stack_new(sizeof(type), #type, 10))

#define	dalib_stack_new_reserve_capacity(type, capacity)	\
	(_dalib_helper_stack_new(sizeof(type), #type, capacity))

#define	dalib_stack_free(stack)	\
	(_dalib_helper_stack_free(&stack))

#define	dalib_stack_is_empty(stack)	\
	(_dalib_helper_stack_is_empty(&stack))

#define	dalib_stack_push_back(stack, type, value)	\
	{	\
		dalib_stack_header_t	*header = (dalib_stack_header_t *)((intptr_t *)stack - _dalib_helper_stack_header_length); \
		if (header->length >= header->capacity) {	\
			stack = _dalib_helper_stack_grow(stack);	\
		}	\
		header = (dalib_stack_header_t *)((intptr_t *)stack - _dalib_helper_stack_header_length); \
		type val = value;	\
		*(type *)(stack + header->length++) = val;	\
	}

#define	dalib_stack_pop_back(stack)	\
	{	\
		dalib_stack_header_t	*header = (dalib_stack_header_t *)((intptr_t *)stack - _dalib_helper_stack_header_length); \
		if (header->length > 0) {	\
			header->length--;	\
		}	\
	}

#define	dalib_stack_get_header(stack)	\
	(_dalib_helper_stack_get_header(stack))

#define	dalib_stack_get_type_name(stack)	\
	(_dalib_helper_stack_get_header(stack)->type_name)

#define	dalib_stack_get_element_size(stack)	\
	(_dalib_helper_stack_get_header(stack)->elem_size)

#define	dalib_stack_get_length(stack)	\
	(_dalib_helper_stack_get_header(stack)->length)

#define	dalib_stack_get_capacity(stack)	\
	(_dalib_helper_stack_get_header(stack)->capacity)

#define	dalib_stack_peek(stack, type)	\
	(_dalib_helper_stack_is_empty(&stack) ? (type){ 0 } : *((type *)stack + (dalib_stack_get_length(stack)) - 1))

#define	dalib_stack_reverse(stack, type)	\
	{	\
		if (stack && !_dalib_helper_stack_is_empty(&stack)) {	\
			dalib_stack_header_t *header = (dalib_stack_header_t *)((intptr_t *)stack - _dalib_helper_stack_header_length); \
			type *front = (type *)stack;	\
			type *back = front + header->length - 1;	\
				\
			for (; front < back; ++front, --back) {	\
				type temp = *front;	\
				*front = *back;	\
				*back = temp;	\
			}	\
		}	\
	}



#endif

