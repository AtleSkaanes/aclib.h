#ifndef __ACLIB_H
#define __ACLIB_H

#include <stdio.h>

// LIST OF DEFINES
// - ACLIB_IMPLEMENTATION
// - ACLIB_STRIP_PREFIX
// - ACLIBDEF
// - ACLIB_VEC_GROWTH_RATE
// - ACLIB_VEC_START_CAP
// - ACLIB_ASSERT_FN
// - ACLIB_CALLOC_FN
// - ACLIB_MALLOC_FN
// - ACLIB_REALLOC_FN
// - ACLIB_FREE_FN
// - ACLIB_LOG_FN

// LIST OF FEATURES
// - Generic Vector
// - Generic Slice
// - String
// - Log
// - Result
//
// LIST OF PLANNED FEATURES
// - Option
// - Arena
// - Cmd runner

#ifndef ACLIBDEF
#define ACLIBDEF
#endif

#ifndef ACLIB_ASSERT_FN
#include <assert.h>
#define ACLIB_ASSERT_FN assert
#endif

#ifndef ACLIB_MALLOC_FN
#include <stdlib.h>
#define ACLIB_MALLOC_FN malloc
#endif

#ifndef ACLIB_CALLOC_FN
#include <stdlib.h>
#define ACLIB_CALLOC_FN calloc
#endif

#ifndef ACLIB_REALLOC_FN
#include <stdlib.h>
#define ACLIB_REALLOC_FN realloc
#endif


#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*          *
 *  VECTOR  *
 *          */
// CONFIG DEFINES:
//  - ACLIB_VEC_GROWTH
//  - ACLIB_VEC_START_CAP
//
// CONST DEFINES:
//  - AL_VEC_FOREACH
//
// TYPES AND TYPE MACROS:
//  - Al_VecDef(T)
//
// FUNCTIONS/MACROS:
//  - al_vec_owned_slice(vec)
//  - al_vec_slice_range(arr, start, end)
//  - al_vec_from(*arr, size)
//  - al_vec_push(*vec, item)
//  - al_vec_unshift(*vec, item)
//  - al_vec_append(*vec, *arr, size)
//  - al_vec_prepend(*vec, *arr, size)
//  - al_vec_pop(*vec)
//  - al_vec_shift(*vec, *out)
//  - al_vec_clone_items(vec)
//      - todo: al_vec_drain(*vec)
//      - todo: al_vec_drain_range(*str, start, end)
//      - todo: al_vec_remove(*vec, idx)
//      - todo: al_vec_remove_range(*vec, start, end)
//  - al_vec_empty(*vec)
//      - todo: al_vec_split_at(vec, idx)
//  - al_vec_free(vec)
//  - al_vec_ensure_cap(vec, new_cap)
//
//
// USAGE:
//  # DEFINING
//  Define a vector type with the `VecDef(T)` macro, e.g:
//  ```c
//  typedef Al_VecDef(int) IntVec;
//  ```
//
//  # INITIALIZING
//  To create such a vector, you can zero intialize it:
//  ```c
//  IntVec ivec = {0};
//  ```
//
//  # USING
//  After you have a vector initialized, you can manipulate it with macros. Such as `al_vec_push()`
//  for pushing a new element
//  ```c
//  IntVec ivec = {0};
//
//  al_vec_push(&ivec, 1); // -> ivec = { 1 }
//  al_vec_push(&ivec, 2); // -> ivec = { 1, 2 }
//  al_vec_push(&ivec, 3); // -> ivec = { 1, 2, 3 }
//  ```
//
//  # FREEING
//  Remember to free the vector after use with `al_vec_free()`, to avoid memory leaks
//  ```c
//  al_vec_free(al);
//  ```

#ifndef ACLIB_VEC_GROWTH_RATE
/// The rate of vectors growth
#define ACLIB_VEC_GROWTH_RATE 1.5
#endif

#ifndef ACLIB_VEC_START_CAP
/// The starting capacity of the vector
#define ACLIB_VEC_START_CAP 10
#endif

/// Define a Vector struct with the given inner type T
#define Al_VecDef(T) \
    struct           \
    {                \
        T* items;    \
        size_t len;  \
        size_t cap;  \
    }

/// Iterate over a vector or slice
#define AL_VEC_FOREACH(T, vec, elem) \
    for (T* elem = (vec).items; (elem) < (vec).items + (vec).len; (elem)++)

/// Create an owned slice from a vector.
/// The caller is responsible for freeing the new slice with `al_slice_free()` after use
#define al_vec_owned_slice(vec) al_slice_from((al_vec_clone_items((vec))), (vec).len)

// Slice a vector in a range from start to end.
// This does not clone or copy any of the elements of the vector, and the vector is still
// responsible for the memory
#define al_vec_slice_range(T, arr, start, end)                           \
    (((start) < (arr).len || (end) < (arr).len)                          \
         ? ((T){.items = (arr).items + (start), .len = (end) - (start)}) \
         : ((T){0}))

/// Create a new vector from a pointer or array. This will clone the contents of the pointer or
/// array.
/// The caller is responsible for freeing the new vector with `al_vec_free()`
#define al_vec_from(arr, size)                                      \
    {                                                               \
        .items = __aclib_clone_arr((arr), (size) * sizeof(*(arr))), \
        .len = (size),                                              \
        .cap = (size),                                              \
    }

/// Push an item to a vector
#define al_vec_push(vec, item)                    \
    {                                             \
        al_vec_ensure_cap((vec), (vec)->len + 1); \
        (vec)->items[(vec)->len] = (item);        \
        (vec)->len += 1;                          \
    }


/// Add an element to the front of the vector
#define al_vec_unshift(vec, item)                           \
    {                                                       \
        al_vec_ensure_cap((vec), (vec)->len + 1);           \
        memcpy((vec)->items + 1, (vec)->items, (vec)->len); \
        (vec)->items[0] = (item);                           \
        (vec)->len++;                                       \
    }


/// Append a list of items to a vector
#define al_vec_append(vec, arr, size)                  \
    {                                                  \
        al_vec_ensure_cap((vec), (vec)->len + (size)); \
        for (size_t i = 0; i < (size); i++)            \
            (vec)->items[(vec)->len + i] = (arr)[i];   \
        (vec)->len += (size);                          \
    }

/// Prepend a list of items to the front of the vector
#define al_vec_prepend(vec, arr, size)                                             \
    {                                                                              \
        al_vec_ensure_cap((vec), (vec)->len + (size));                             \
        /* Move all current items to make space in front */                        \
        memmove((vec)->items + (size), (vec)->items, (vec)->len * sizeof(*(arr))); \
        /* Add the items from the arr */                                           \
        memcpy((vec)->items, (arr), (size) * sizeof(*(arr)));                      \
        (vec)->len += (size);                                                      \
    }


/// Pops and returns the last element in the vector.
/// This asserts that the vector has an element that can be popped
#define al_vec_pop(vec)                                                                     \
    (ACLIB_ASSERT_FN((vec)->len >= 1 &&                                                     \
                     "Vector failed to pop, expected length of >= 1, but got length of 0"), \
     (vec)->len -= 1, ((vec)->items[(vec)->len]))

// Remove the first element in the vector, and puts it into out.
/// This asserts that the vector has an element that can be shifted
#define al_vec_shift(vec, out)                                                                   \
    {                                                                                            \
        ACLIB_ASSERT_FN((vec)->len >= 1 &&                                                       \
                        "Vector failed to shift, expected length of >= 1, but got length of 0"); \
        *(out) = (vec)->items[0];                                                                \
        memcpy((vec)->items, (vec)->items + 1, (vec)->len - 1);                                  \
        (vec)->len -= 1;                                                                         \
    }

/// Clone the items of a vector, returning them as an allocated pointer.
/// The caller is responsible for freeing the new pointer
#define al_vec_clone_items(vec) \
    ((vec).cap > 0 ? __aclib_clone_arr((vec).items, (vec).len * sizeof((vec).items[0])) : 0)

/// Empty the given vector. This does not free or remove any memory
#define al_vec_empty(vec) (vec)->len = 0

/// Free the given vector and its contents
#define al_vec_free(vec)   \
    {                      \
        free((vec).items); \
        (vec).items = 0;   \
        (vec).len = 0;     \
        (vec).cap = 0;     \
    }


/// Ensure that a vector has atleast the given capacity.
#define al_vec_ensure_cap(vec, new_cap)                                                      \
    if ((vec)->cap < new_cap)                                                                \
    {                                                                                        \
        size_t calced_cap;                                                                   \
        if ((vec)->cap == 0)                                                                 \
        {                                                                                    \
            calced_cap = ACLIB_VEC_START_CAP;                                                \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
            calced_cap = (new_cap * ACLIB_VEC_GROWTH_RATE);                                  \
        }                                                                                    \
        (vec)->items = ACLIB_REALLOC_FN((vec)->items, calced_cap * sizeof((vec)->items[0])); \
                                                                                             \
        if ((vec)->items == NULL)                                                            \
        {                                                                                    \
            al_log(ACLIB_ERR, "Failed to reallocate vector\n");                              \
            exit(EXIT_FAILURE);                                                              \
        }                                                                                    \
                                                                                             \
        (vec)->cap = new_cap;                                                                \
    }

void* __aclib_clone_arr(void* ptr, size_t size);

/* END OF VECTOR DECL */



/*          *
 *  SLICES  *
 *          */
// CONFIG DEFINES:
//  -
//
// CONST DEFINES:
//  -
//
// TYPES AND TYPE MACROS:
//  - Al_SliceDef(T)
//
// FUNCTIONS AND MACROS:
//  - al_slice_from(arr, size)
//  - al_slice_free(slice)
//      - al_slice_clone(slice)
//
// USAGE:
//  # DEFINING
//  Define a slice type with the `VecDef(T)` macro, e.g:
//  ```c
//  typedef Al_SliceDef(int) IntSlice;
//  ```
//
//  # INITIALIZING
//  To create such a vector, you can zero intialize it:
//  ```c
//  IntSlice islice = {0};
//  ```

/// Define a Slice struct with the given inner type T
#define Al_SliceDef(T) \
    struct             \
    {                  \
        T* items;      \
        size_t len;    \
    }


/// Create a new slice from a pointer, and a len.
/// This will not clone or copy the items in the array, so it will not own the contents.
/// Do not use `al_slice_free()` on a slice from this, unless you know what you are doing, as that
/// would free the original contents.
#define al_slice_from(arr, size) \
    {                            \
        .items = arr,            \
        .len = size,             \
    }


/// Free a slice. Be sure that the slice owns its contents before calling this, as it otherwise can
/// create memory issues
#define al_slice_free(slice)  \
    {                         \
        free((slice)->items); \
        (slice)->len = 0;     \
    }

/* END OF SLICE DECL */



/*          *
 *  STRING  *
 *          */
// CONFIG DEFINES:
//  -
//
// CONST DEFINES
//  - AL_STR_FMT
//  - AL_STR_ARG
//
// TYPES AND TYPE MACROS:
//  - Al_String
//  - Al_StrSlice
//  - Al_StrVec
//
// FUNCTIONS AND MACROS:
//  - al_str_slice_with_len(len)
//  - al_str_slice_from(chs)
//  - al_str_owned_slice(str)
//  - al_str_slice_clone(slice)
//  - al_str_slice_range(str, start, end)
//  - al_str_slice_free(*slice)
//
//  - al_str_with_capacity(capacity)
//  - al_str_from(*chs)
//  - al_str_push(*str, ch)
//  - al_str_unshift(*str, ch)
//  - al_str_append(*str, *chs)
//  - al_str_appendf(*str, *fmt, ...)
//  - al_str_prepend(*str, *chs)
//  - al_str_prependf(*str, *chs)
//  - al_str_pop(*str)
//  - al_str_shift(*str)
//  - al_str_clone_chars(str)
//  - al_str_drain(*str);
//  - al_str_drain_range(*str, start, end);
//  - al_str_remove(*str, idx)
//  - al_str_remove_range(*str, start, end)
//  - al_str_empty(*str)
//  - al_str_free(*str)
//  - al_str_ensure_cap(*str, new_cap)
//  - al_str_trim_front(*str)
//  - al_str_trim_back(*str)
//  - al_str_trim(*str)
//  - al_str_split_by(str, delim)
//  - al_str_split_by_once(str, delim)
//  - al_str_split_by_many(str, *delims)
//  - al_str_split_at(str, idx)
//  - al_str_read_file*buffer, *file)
//  - al_str_read_lines(*linebuffer, *file)
//
// USAGE:
//  # DEFINING
//  Define a slice type with the `VecDef(T)` macro, e.g:
//  ```c
//  typedef Al_SliceDef(int) IntVec;
//  ```
//
//  # INITIALIZING
//  To create such a vector, you can zero intialize it:
//  ```c
//  IntVec ivec = {0};
//  ```
//
//  # USING
//  After you have a vector initialized, you can manipulate it with macros. Such as `al_vec_push()`
//  for pushing a new element
//  ```c
//  IntVec ivec = {0};
//
//  al_vec_push(&ivec, 1); // -> ivec = { 1 }
//  al_vec_push(&ivec, 2); // -> ivec = { 1, 2 }
//  al_vec_push(&ivec, 3); // -> ivec = { 1, 2, 3 }
//  ```
//
//  # FREEING
//  Remember to free the vector after use with `al_vec_free()`, to avoid memory leaks
//  ```c
//  al_vec_free(al);
//  ```

/// A dynamic and owned string
typedef struct Al_String
{
    /// The characters of the string. Can be used as a cstr
    char* chars;
    /// The length of the string
    size_t len;
    /// The capacity of the string
    size_t cap;
} Al_String;


/// A string slice, which holds a cstr and a length
typedef struct Al_StrSlice
{
    /// The characters of the string slice. Can be used as a cstr
    char* chars;
    /// The length of the string slice
    size_t len;
} Al_StrSlice;

/// A format string used to print strings and string slices properly. Remember to use `AL_STR_ARG()`
/// as the format argument.
#define AL_STR_FMT "%.*s"
/// The arg used
#define AL_STR_ARG(str) (int)(str).len, (str).chars

/// A vector containing string slices.
/// To avoid leaking memory, remember to free each individual string slice by themselves, as
/// `vec_free` will not do this for you.
typedef Al_VecDef(Al_StrSlice) Al_StrVec;

/// Allocate a new empty string slice with a specific length. The caller is responsible for freeing
/// the memory with `al_slice_free()`
ACLIBDEF Al_StrSlice al_str_slice_with_len(size_t len);

/// Get a slice to a c string, this will not copy or clone the chars, just point to them.
ACLIBDEF Al_StrSlice al_str_slice_from(char* chs);

/// Get an owned slice to the contents of a string. The caller is responsible for freeing the new
/// slice with `al_str_slice_free(slice)`
ACLIBDEF Al_StrSlice al_str_owned_slice(Al_String str);

/// Clone a string slice and its chars.
/// The caller i responsible for freeing the slice with `al_str_slice_free()`
ACLIBDEF Al_StrSlice al_str_slice_clone(Al_StrSlice slice);

/// Get a slice from a string in a specific range. This does not copy or clone the chars in the
/// range.
ACLIBDEF Al_StrSlice al_str_slice_range(Al_String str, size_t start, size_t end);

/// Free a string slice and its memory. This should *only* be called if the slice owns its own
/// characters
ACLIBDEF void al_str_slice_free(Al_StrSlice* slice);

/// Allocates a new string with a specific capacity. The caller is responsible for freeing the
/// string with `al_str_free()`
ACLIBDEF Al_String al_str_with_capacity(size_t capacity);

/// Create a new string from a cstr. This will clone the contents of the cstr.
/// The caller is responsible for freeing the new string with `al_str_free()`
ACLIBDEF Al_String al_str_from(char* chs);

/// Push a char unto the end of a string
ACLIBDEF void al_str_push(Al_String* str, char ch);

/// Adds a char to the start of a string
ACLIBDEF void al_str_unshift(Al_String* str, char ch);

/// Append a cstr unto the end of a string
ACLIBDEF void al_str_append(Al_String* str, char* chs);

/// Append a formatted cstr unto the end of a string
ACLIBDEF void al_str_appendf(Al_String* str, const char* fmt, ...);

/// Prepend a cstr to the front of a string
ACLIBDEF void al_str_prepend(Al_String* str, char* chs);

/// Prepend a formatted cstr to the front of a string
ACLIBDEF void al_str_prependf(Al_String* str, const char* fmt, ...);

/// Pop and return the last char of a string.
/// This asserts that the string has a char that can be popped
ACLIBDEF char al_str_pop(Al_String* str);

/// Remove and return the first char of a string.
/// This asserts that the string has a char that can be popped
ACLIBDEF char al_str_shift(Al_String* str);

/// Clone the chars behind a string, and return them as an owned string slice.
/// The caller is responsible for freeing the allocated cstr
ACLIBDEF char* al_str_clone_chars(Al_String str);

/// Puts all the contents of a string into a slice, removing all of the contents of the original
/// string.
/// The slice is now the owner of the memory.
/// The caller is responsible for freeing the string slice with `al_str_slice_free()`
ACLIBDEF Al_StrSlice al_str_drain(Al_String* str);

/// Puts all the contents of a string in a given range into a slice, removing the elements from the
/// original string. The slice is now the owner of the removed memory. The caller is responsible for
/// freeing the string slice with `al_str_slice_free()`
ACLIBDEF Al_StrSlice al_str_drain_range(Al_String* str, size_t start, size_t end);

/// Remove a char at a given index from a string
ACLIBDEF void al_str_remove(Al_String* str, size_t idx);

/// Remove a set of chars in a given range, from a string
ACLIBDEF void al_str_remove_range(Al_String* str, size_t start, size_t end);

/// Empty the given string. This does not free or remove any memory
ACLIBDEF void al_str_empty(Al_String* str);

/// Free the given vector and its contents
ACLIBDEF void al_str_free(Al_String* str);

/// Ensure that a string has atleast the given capacity.
ACLIBDEF void al_str_ensure_cap(Al_String* str, size_t new_cap);

/// Trim all whitespace from the front of a string
ACLIBDEF void al_str_trim_front(Al_String* str);

/// Trim all whitespace from the back of the string
ACLIBDEF void al_str_trim_back(Al_String* str);

/// Trim all whitespace from the front and back of a string
ACLIBDEF void al_str_trim(Al_String* str);

/// Split a string by a delimeter
ACLIBDEF Al_StrVec al_str_split_by(Al_String str, char delim);

/// Split a string by a set of delimeters
ACLIBDEF Al_StrVec al_str_split_by_many(Al_String str, char* delims);

/// Split a string by a delimeter once
ACLIBDEF Al_StrVec al_str_split_by_once(Al_String str, char delim);

/// Split a string at an index
ACLIBDEF Al_StrVec al_str_split_at(Al_String str, size_t idx);

/// Reads a file into a string buffer, and returns the amount of bytes read.
ACLIBDEF size_t al_str_read_file(Al_String* buffer, FILE* file);

// Reads all the lines in a file into a line buffer, and returns the amount of bytes read.
ACLIBDEF size_t al_str_read_lines(Al_StrVec* linebuffer, FILE* file);

/* END OF STRING DECL */



/*         *
 *  ASCII  *
 *         */
// CONFIG DEFINES:
//  -
//
// CONST DEFINES:
//  -
//
// TYPES AND TYPE MACROS:
//  -
//
// FUNCTIONS AND MACROS:
//  - al_ascii_is_alphabetic(ch)
//  - al_ascii_is_numeric(ch)
//  - al_ascii_is_alphanumeric(ch)
//  - al_ascii_is_whitespace(ch)
//  - al_ascii_is_uppercase(ch)
//  - al_ascii_is_lowercase(ch)
//  - al_ascii_to_uppercase(ch)
//  - al_ascii_to_lowercase(ch)
//
// USAGE:

/// Checks if an ascii char is alphabetic, i.e. [a-zA-Z]
char al_ascii_is_alphabetic(char ch);
/// Checks if an ascii char is numeric, i.e. [0-9]
bool al_ascii_is_numeric(char ch);
/// Checks if an ascii char is alphanumeric, i.e. [a-zA-Z0-9]
bool al_ascii_is_alphanumeric(char ch);
/// Checks if an ascii char is whitespace, e.g. '\n', '\t', ' '
bool al_ascii_is_whitespace(char ch);
/// Checks if an ascii char is uppercase
bool al_ascii_is_uppercase(char ch);
/// Checks if an ascii char is lowercase
bool al_ascii_is_lowercase(char ch);
/// Returns the uppercase version of an ascii char
char al_ascii_to_uppercase(char ch);
/// Returns the lowercase version of an ascii char
char al_ascii_to_lowercase(char ch);

/* END OF ASCII DECL */



/*           *
 *  LOGGING  *
 *           */
// CONFIG DEFINES:
//  - ALIB_LOG_FN
//  - ALIB_LOG_FD
//
// CONST DEFINES:
//  -
//
// TYPES AND TYPE MACROS:
//  - Al_LogLevel
//
// FUNCTIONS AND MACROS:
//  - al_log(loglvl, fmt, ...)
//  - al_todo(msg)
//
// USAGE:

#ifndef ACLIB_LOG_FN
/// Sets the function that `al_log` will write to. The function must have the signature
/// `void (Al_LogLevel loglvl, const char* fmt, ...)`
#define ACLIB_LOG_FN __aclib_default_log_fn
#endif

/// The ansi code for red. Used by aclib's default error logging
#define _ACLIB_RED "\x1b[31m"
/// The ansi code for green. Used by aclib's default success logging
#define _ACLIB_GREEN "\x1b[32m"
/// The ansi code for yellow. Used by aclib's default warn logging
#define _ACLIB_YELLOW "\x1b[33m"
/// The ansi code for blue. Used by aclib's default debug logging
#define _ACLIB_BLUE "\x1b[34m"
/// The ansi code for clearing all styles
#define _ACLIB_CLEAR "\x1b[0m"

/// The loglevel used by al_log()
typedef enum Al_LogLevel
{
    ACLIB_DEBUG,
    ACLIB_INFO,
    ACLIB_WARN,
    ACLIB_SUCCESS,
    ACLIB_ERR,
    ACLIB_NO_LOGS,
} Al_LogLevel;

/// The current minimun log level for `al_log`
extern Al_LogLevel aclib_log_level;
/// the file descriptor that aclibs logging default implementation will write to
extern FILE* aclib_log_fd;
/// Sets the function that `al_log` will write to. The function must have the signature
/// `void (Al_LogLevel loglvl, const char* fmt, ...)`
extern void (*aclib_log_fn_ptr)(Al_LogLevel, const char*, ...);

/// The default logging function for aclib
ACLIBDEF void __aclib_default_log_fn(Al_LogLevel loglvl, const char* fmt, ...);
/// Print a log messae
#define al_log (aclib_log_fd = (!aclib_log_fd) ? stderr : aclib_log_fd, *aclib_log_fn_ptr)

#define al_todo(msg) (al_log(ACLIB_ERR, "%s:%d: TODO: %s\n", __FILE__, __LINE__, (msg)), abort())

/* END OF LOGGING DECL */



/*          *
 *  RESULT  *
 *          */
// CONFIG DEFINES:
//  -
//
// CONST DEFINES
//  -
//
// TYPES AND TYPE MACROS:
//  - Al_ResTag
//  - Al_ResDef(T, E)
//
// FUNCTIONS AND MACROS:
//  - al_res_ok(val)
//  - al_res_err(val)
//  - al_res_try(res)
//  - al_res_unwrap(res)
//  - al_res_unwrap_err(res)
//  - al_res_unwrap_or(res, or)
//  - al_res_unwrap_or_else(res, or_fn)
//  - al_res_map(T, res, fn)
//  - al_res_map_err(T, res, fn)
//
// USAGE:

/// The tag behind the Al_Result
typedef enum
{
    AL_RES_OK,
    AL_RES_ERR,
} Al_ResTag;

/// Define a new Result type with an ok type (T) and an err type (E)
#define Al_ResDef(T, E)                                                               \
    struct                                                                            \
    {                                                                                 \
        /* The tag of this result. Determines what kind of value this object holds */ \
        /* AL_RES_OK => res.ok */                                                     \
        /* AL_RES_ERR => res.err */                                                   \
        Al_ResTag tag;                                                                \
        union                                                                         \
        {                                                                             \
            T ok;                                                                     \
            E err;                                                                    \
        };                                                                            \
    }


/// Construct a new Result value, set to ok, and with a given value
#define al_res_ok(val) {.tag = AL_RES_OK, .ok = (val)}

/// Construct a new Result value, set to err, and with a given error value
#define al_res_err(err_val) {.tag = AL_RES_ERR, .err = (err_val)}

/// Unwrap a result, or return the error
#define al_res_try(T, res) \
    ((res).tag == AL_RES_OK ? (res).ok : (({ return (T)al_res_err((res).err); }), 0))

/// Unwrap a result ok value. Asserts that the result has the ok tag
#define al_res_unwrap(res)                                                                        \
    (ACLIB_ASSERT_FN((res).tag == AL_RES_OK && "Tried to unwrap result, but got tag AL_RES_ERR"), \
     res.ok)

/// Unwrap a result error value. Asserts that the result has the err tag
#define al_res_unwrap_err(res)                                               \
    (ACLIB_ASSERT_FN((res).tag == AL_RES_ERR &&                              \
                     "Tried to unwrap result error, but got tag AL_RES_OK"), \
     res.ok)

/// Unwrap a result ok value, or give the given or value
#define al_res_unwrap_or(res, or) ((res).tag == AL_RES_OK ? (res).ok : (or))

/// Unwrap a result ok value, or give a value, returned from the ok_fn function
#define al_res_unwrap_or_else(res, or_fn) ((res).tag == AL_RES_OK ? (res).tag : (or_fn)())

/// Run a function on an results ok value, or give the error
#define al_res_map(T, res, fn) \
    ((res).tag == AL_RES_OK ? (T)al_res_ok((fn)((res).ok)) : (T)al_res_err((res).err))

/// Run a function on an results err value, or give the ok valie
#define al_res_map_err(T, res, fn) \
    ((res).tag == AL_RES_ERR ? (T)al_res_err((fn)((res).err)) : (T)al_res_ok((res).ok))


/* END OF RESULT DECL */



/*                        *
 *  ACLIB IMPLEMENTATION  *
 *                        */
#define ACLIB_IMPLEMENTATION
#ifdef ACLIB_IMPLEMENTATION



/*                         *
 *  VECTOR IMPLEMENTATION  *
 *                         */

ACLIBDEF void* __aclib_clone_arr(void* ptr, size_t size)
{
    void* buffer = malloc(size);
    memcpy(buffer, ptr, size);
    return buffer;
}

/* END OF VECTOR IMPLEMENTATION */



/*                         *
 *  STRING IMPLEMENTATION  *
 *                         */

ACLIBDEF Al_StrSlice al_str_slice_with_len(size_t len)
{
    return (Al_StrSlice){
        .chars = (char*)ACLIB_CALLOC_FN(len + 1, sizeof(char)),
        .len = len,
    };
}

ACLIBDEF Al_StrSlice al_str_slice_from(char* chs)
{
    return (Al_StrSlice){
        .chars = chs,
        .len = strlen(chs),
    };
}

ACLIBDEF Al_StrSlice al_str_owned_slice(Al_String str)
{
    if (str.cap == 0)
        return (Al_StrSlice){0};

    char* buffer = al_str_clone_chars(str);
    return al_str_slice_from(buffer);
}

ACLIBDEF Al_StrSlice al_str_slice_clone(Al_StrSlice slice)
{
    char* chars = (char*)ACLIB_MALLOC_FN((slice.len + 1) * sizeof(char));
    memcpy(chars, slice.chars, slice.len * sizeof(char));

    chars[slice.len] = '\0';
    return (Al_StrSlice){.chars = chars, .len = slice.len};
}

ACLIBDEF Al_StrSlice al_str_slice_range(Al_String str, size_t start, size_t end)
{
    if (start >= str.len || end > str.len || start >= end)
        return (Al_StrSlice){0};

    return (Al_StrSlice){
        .chars = str.chars + start,
        .len = end - start,
    };
}

ACLIBDEF void al_str_slice_free(Al_StrSlice* slice)
{
    free(slice->chars);
    slice->len = 0;
}

ACLIBDEF Al_String al_str_with_capacity(size_t capacity)
{
    return (Al_String){
        .chars = (char*)ACLIB_CALLOC_FN(capacity + 1, sizeof(char)),
        .len = 0,
        .cap = capacity,
    };
}

ACLIBDEF Al_String al_str_from(char* chs)
{
    Al_String str = {0};
    al_str_append(&str, chs);
    return str;
}

ACLIBDEF void al_str_push(Al_String* str, char ch)
{
    al_str_ensure_cap(str, str->len + 1);

    str->chars[str->len] = ch;
    str->chars[str->len + 1] = '\0';
    str->len += 1;
}

// LEN: 9:
//
// 'NUMBER: 4'
// 'NNUMBER: 4'
// '[NUMBER: 4'

ACLIBDEF void al_str_unshift(Al_String* str, char ch)
{
    al_str_ensure_cap(str, str->len + 1);
    memmove(str->chars + 1, str->chars, str->len * sizeof(char));
    str->chars[0] = ch;
    str->len += 1;
    str->chars[str->len] = '\0';
}

ACLIBDEF void al_str_append(Al_String* str, char* chs)
{
    size_t size = strlen(chs);
    al_str_ensure_cap(str, str->len + size);

    for (size_t i = 0; i < size; i++)
        str->chars[str->len + i] = chs[i];

    str->chars[str->len + size] = '\0';
    str->len += size;
}

ACLIBDEF void al_str_appendf(Al_String* str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t needed = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);

    al_str_ensure_cap(str, str->len + needed);

    char* dest = str->chars + str->len;
    va_start(args, fmt);
    vsprintf(dest, fmt, args);
    va_end(args);

    // sprintf automatically adds '\0', so take account for it
    str->len += needed - 1;
}

ACLIBDEF void al_str_prepend(Al_String* str, char* chs)
{
    size_t size = strlen(chs);
    al_str_ensure_cap(str, str->len + size);

    memcpy(str->chars + size, str->chars, str->len * sizeof(char));
    memcpy(str->chars, chs, size * sizeof(char));

    str->chars[str->len + size] = '\0';
    str->len += size;
}

ACLIBDEF void al_str_prependf(Al_String* str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t needed = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);

    al_str_ensure_cap(str, str->len + needed);

    memcpy(str->chars + needed - 1, str->chars, str->len * sizeof(char));

    Al_StrSlice format_buf = al_str_slice_with_len(needed - 1);
    va_start(args, fmt);
    vsprintf(format_buf.chars, fmt, args);
    va_end(args);

    memcpy(str->chars, format_buf.chars, format_buf.len * sizeof(char));

    al_str_slice_free(&format_buf);

    // sprintf automatically adds '\0', so take account for it
    str->len += needed - 1;
}

ACLIBDEF char al_str_pop(Al_String* str)
{
    ACLIB_ASSERT_FN(str->len >= 1 &&
                    "String failed to pop, expected length of >= 1, but got length of 0");
    str->len -= 1;
    return str->chars[str->len];
}

ACLIBDEF char al_str_shift(Al_String* str)
{
    ACLIB_ASSERT_FN(str->len >= 1 &&
                    "String failed to shift, expected length of >= 1, but got length of 0");

    char shifted = str->chars[0];

    memcpy(str->chars, str->chars + 1, (str->len - 1) * sizeof(char));
    str->len -= 1;

    return shifted;
}

ACLIBDEF char* al_str_clone_chars(Al_String str)
{
    char* buffer = (char*)malloc((str.len + 1) * sizeof(char));

    if (buffer == NULL)
    {
        al_log(ACLIB_ERR, "Failed to clone string chars\n");
        exit(EXIT_FAILURE);
    }

    memcpy(buffer, str.chars, str.len * sizeof(char));
    buffer[str.len] = '\0';

    return buffer;
}

ACLIBDEF Al_StrSlice al_str_drain(Al_String* str)
{
    Al_StrSlice slice = al_str_slice_from(str->chars);

    *str = (Al_String){0};

    return slice;
}

ACLIBDEF Al_StrSlice al_str_drain_range(Al_String* str, size_t start, size_t end)
{
    Al_StrSlice slice = al_str_slice_range(*str, start, end);
    slice = al_str_slice_clone(slice);

    al_str_remove_range(str, start, end);

    return slice;
}

ACLIBDEF void al_str_remove(Al_String* str, size_t idx)
{
    if (idx >= str->len)
        return;

    al_str_remove_range(str, idx, idx + 1);
}

ACLIBDEF void al_str_remove_range(Al_String* str, size_t start, size_t end)
{
    if (start >= str->len || end > str->len || start >= end)
        return;

    if (end == str->len)
    {
        str->len = start;
        str->chars[str->len] = '\0';
        return;
    }

    memmove(str->chars + start, str->chars + end, (str->len - end) * sizeof(char));
    str->len -= (end - start);
    str->chars[str->len] = '\0';
}

ACLIBDEF void al_str_empty(Al_String* str)
{
    str->len = 0;
    if (str->chars)
        str->chars[0] = '\0';
}

ACLIBDEF void al_str_free(Al_String* str)
{
    if (str->chars == NULL)
        return;

    free(str->chars);
    str->chars = (char*)0;
    str->len = 0;
    str->cap = 0;
}

ACLIBDEF void al_str_ensure_cap(Al_String* str, size_t new_cap)
{
    if (str->cap < new_cap + 1)
    {
        // + 1 for the '\0'
        str->chars = (char*)ACLIB_REALLOC_FN(str->chars, ((new_cap * ACLIB_VEC_GROWTH_RATE) + 1) *
                                                             sizeof(char));
        str->cap = new_cap;

        if (str->chars == NULL)
        {
            al_log(ACLIB_ERR, "Failed to reallocate string\n");
            exit(EXIT_FAILURE);
        }

        str->chars[str->cap] = '\0';
    }
}


#define INTERNAL_BUF_SIZE 256
ACLIBDEF size_t al_str_read_file(Al_String* buffer, FILE* file)
{
    size_t bytes_read = 0;
    char char_buf[INTERNAL_BUF_SIZE];
    while (fgets(char_buf, INTERNAL_BUF_SIZE, file))
    {
        bytes_read += strlen(char_buf);
        al_str_append(buffer, char_buf);
    }

    return bytes_read;
}

ACLIBDEF size_t al_str_read_lines(Al_StrVec* linebuffer, FILE* file)
{
    size_t bytes_read = 0;

    char char_buf[INTERNAL_BUF_SIZE];
    while (fgets(char_buf, INTERNAL_BUF_SIZE, file))
    {
        bytes_read += strlen(char_buf);

        Al_String line = al_str_from(char_buf);
        size_t old_size = strlen(char_buf);
        while (char_buf[strlen(char_buf) - 1] != '\n')
        {
            fgets(char_buf, INTERNAL_BUF_SIZE, file);
            if (strlen(char_buf) == old_size)
                break;
            al_str_append(&line, char_buf);
        }

        // pop the '\n'
        if (line.chars[line.len - 1] == '\n')
            al_str_pop(&line);

        al_vec_push(linebuffer, al_str_owned_slice(line));
        al_str_free(&line);
    }

    return bytes_read;
}
#undef INTERNAL_BUF_SIZE

ACLIBDEF void al_str_trim_front(Al_String* str)
{
    if (str->len == 0)
        return;

    size_t end = 0;
    while (end < str->len && al_ascii_is_whitespace(str->chars[end]))
        end++;

    al_str_remove_range(str, 0, end);
}

/// Trim all whitespace from the back of the string
ACLIBDEF void al_str_trim_back(Al_String* str)
{
    if (str->len == 0)
        return;

    size_t start = str->len;
    while (start > 0 && al_ascii_is_whitespace(str->chars[start - 1]))
        start--;

    al_str_remove_range(str, start, str->len);
}

/// Trim all whitespace from the front and back of a string
ACLIBDEF void al_str_trim(Al_String* str)
{
    al_str_trim_back(str);
    al_str_trim_front(str);
}

/// Split a string by a delimeter
ACLIBDEF Al_StrVec al_str_split_by(Al_String str, char delim)
{
    size_t start = 0;
    size_t end = 0;

    Al_StrVec parts = {0};

    while (end <= str.len)
    {
        if (str.chars[end] == delim)
        {
            Al_StrSlice slice = al_str_slice_range(str, start, end);
            slice = al_str_slice_clone(slice);

            al_vec_push(&parts, slice);
            end++;
            start = end;
        }
        end++;
    }

    Al_StrSlice slice = al_str_slice_range(str, start, str.len);
    slice = al_str_slice_clone(slice);

    al_vec_push(&parts, slice);

    return parts;
}

/// Split a string by a set of delimeters
ACLIBDEF Al_StrVec al_str_split_by_many(Al_String str, char* delims)
{
    size_t start = 0;
    size_t end = 0;

    Al_StrVec parts = {0};

    while (end < str.len)
    {
        bool is_delim = false;
        for (size_t i = 0; i < strlen(delims); i++)
        {
            if (str.chars[end] == delims[i])
            {
                is_delim = true;
                break;
            }
        }

        if (is_delim)
        {
            Al_StrSlice slice = al_str_slice_range(str, start, end);
            slice = al_str_slice_clone(slice);

            al_vec_push(&parts, slice);
            end++;
            start = end;
        }
        end++;
    }

    Al_StrSlice slice = al_str_slice_range(str, start, str.len);
    slice = al_str_slice_clone(slice);

    al_vec_push(&parts, slice);

    return parts;
}

/// Split a string by a delimeter once
ACLIBDEF Al_StrVec al_str_split_by_once(Al_String str, char delim)
{
    size_t end = 0;

    Al_StrVec parts = {0};

    while (end < str.len)
    {
        if (str.chars[end] == delim)
        {
            Al_StrSlice slice = al_str_slice_range(str, 0, end);
            slice = al_str_slice_clone(slice);

            al_vec_push(&parts, slice);
            break;
        }
        end++;
    }

    Al_StrSlice rest = al_str_slice_range(str, end + 1, str.len);
    rest = al_str_slice_clone(rest);
    al_vec_push(&parts, rest);

    return parts;
}

/// Split a string at an index
ACLIBDEF Al_StrVec al_str_split_at(Al_String str, size_t idx)
{
    Al_StrVec parts = {0};

    if (idx >= str.len)
    {
        al_vec_push(&parts, al_str_owned_slice(str));
        return parts;
    }

    Al_StrSlice first_part = al_str_slice_range(str, 0, idx);
    first_part = al_str_slice_clone(first_part);
    al_vec_push(&parts, first_part);

    Al_StrSlice second_part = al_str_slice_range(str, idx, str.len);
    second_part = al_str_slice_clone(second_part);
    al_vec_push(&parts, second_part);

    return parts;
}

/* END OF STRING IMPLEMENTATION */



/*                        *
 *  ASCII IMPLEMENTATION  *
 *                        */

char al_ascii_is_alphabetic(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool al_ascii_is_numeric(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool al_ascii_is_alphanumeric(char ch)
{
    return al_ascii_is_alphabetic(ch) || al_ascii_is_numeric(ch);
}

bool al_ascii_is_whitespace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f';
}

bool al_ascii_is_uppercase(char ch)
{
    return ch >= 'A' && ch <= 'Z';
}

bool al_ascii_is_lowercase(char ch)
{
    return ch >= 'a' && ch <= 'z';
}

char al_ascii_to_uppercase(char ch)
{
    if (!al_ascii_is_alphabetic(ch))
        return ch;

    // e.g. convert 1000010 (B) to 1100010 (b)
    return ch & ~0x20;
}

char al_ascii_to_lowercase(char ch)
{
    if (!al_ascii_is_alphabetic(ch))
        return ch;

    // e.g. convert 1000010 (B) to 1100010 (b)
    return ch | 0x20;
}

/* END OF ASCII IMPLEMENTATION */



/*                          *
 *  LOGGING IMPLEMENTATION  *
 *                          */

Al_LogLevel aclib_log_level = ACLIB_INFO;
FILE* aclib_log_fd = NULL;
void (*aclib_log_fn_ptr)(Al_LogLevel, const char*, ...) = __aclib_default_log_fn;

ACLIBDEF void __aclib_default_log_fn(Al_LogLevel loglvl, const char* fmt, ...)
{
    if (loglvl < aclib_log_level)
        return;

    switch (loglvl)
    {
        case ACLIB_DEBUG:
            fprintf(aclib_log_fd, _ACLIB_BLUE "debug: " _ACLIB_CLEAR);
            break;
        case ACLIB_INFO:
            fprintf(aclib_log_fd, "info: ");
            break;
        case ACLIB_WARN:
            fprintf(aclib_log_fd, _ACLIB_YELLOW "warning: " _ACLIB_CLEAR);
            break;
        case ACLIB_SUCCESS:
            fprintf(aclib_log_fd, _ACLIB_GREEN "success: " _ACLIB_CLEAR);
            break;
        case ACLIB_ERR:
            fprintf(aclib_log_fd, _ACLIB_RED "error: " _ACLIB_CLEAR);
            break;
        case ACLIB_NO_LOGS:
            return;
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(aclib_log_fd, fmt, args);
    va_end(args);
}

/* END OF LOGGING IMPLEMENTATION */

#endif // ACLIB_IMPLEMENTATION

#endif // __ACLIB_H
