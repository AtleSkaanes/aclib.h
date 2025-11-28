#ifndef __ACLIB_H
#define __ACLIB_H

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#define __USE_GNU // Include execvpe
#include <sys/wait.h>
#include <unistd.h>

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
// - Option
//
// LIST OF PLANNED FEATURES
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

#define _Nullable

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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
//  - Ac_ResTag
//  - Ac_ResDef(T, E)
//
// FUNCTIONS AND MACROS:
//  - ac_res_ok(val)
//  - ac_res_err(val)
//  - ac_res_try(res)
//  - ac_res_unwrap(res)
//  - ac_res_unwrap_err(res)
//  - ac_res_unwrap_or(res, or)
//  - ac_res_unwrap_or_else(res, or_fn)
//  - ac_res_map(T, res, fn)
//  - ac_res_map_err(T, res, fn)
//
// USAGE:

/// The tag behind the Ac_Result
typedef enum Ac_ResTag
{
    AC_RES_OK,
    AC_RES_ERR,
} Ac_ResTag;

/// Define a new Result type with an ok type (T) and an err type (E)
#define Ac_ResDef(T, E)                                                               \
    struct                                                                            \
    {                                                                                 \
        /* The tag of this result. Determines what kind of value this object holds */ \
        /* AC_RES_OK => res.ok */                                                     \
        /* AC_RES_ERR => res.err */                                                   \
        Ac_ResTag tag;                                                                \
        union                                                                         \
        {                                                                             \
            T ok;                                                                     \
            E err;                                                                    \
        };                                                                            \
    }


/// Construct a new Result value, set to ok, and with a given value
#define ac_res_ok(val) {.tag = AC_RES_OK, .ok = (val)}

/// Construct a new Result value, set to err, and with a given error value
#define ac_res_err(err_val) {.tag = AC_RES_ERR, .err = (err_val)}

/// Unwrap a result, or return the error
#define ac_res_try(T, res) \
    ((res).tag == AC_RES_OK ? (res).ok : (({ return (T)ac_res_err((res).err); }), 0))

/// Unwrap a result ok value. Asserts that the result has the ok tag
#define ac_res_unwrap(res)                                                                        \
    (ACLIB_ASSERT_FN((res).tag == AC_RES_OK && "Tried to unwrap result, but got tag AC_RES_ERR"), \
     res.ok)

/// Unwrap a result error value. Asserts that the result has the err tag
#define ac_res_unwrap_err(res)                                               \
    (ACLIB_ASSERT_FN((res).tag == AC_RES_ERR &&                              \
                     "Tried to unwrap result error, but got tag AC_RES_OK"), \
     res.ok)

/// Unwrap a result ok value, or give the given or value
#define ac_res_unwrap_or(res, or) ((res).tag == AC_RES_OK ? (res).ok : (or))

/// Unwrap a result ok value, or give a value, returned from the ok_fn function
#define ac_res_unwrap_or_else(res, or_fn) ((res).tag == AC_RES_OK ? (res).tag : (or_fn)())

/// Run a function on an results ok value, or give the error
#define ac_res_map(T, res, fn) \
    ((res).tag == AC_RES_OK ? (T)ac_res_ok((fn)((res).ok)) : (T)ac_res_err((res).err))

/// Run a function on an results err value, or give the ok valie
#define ac_res_map_err(T, res, fn) \
    ((res).tag == AC_RES_ERR ? (T)ac_res_err((fn)((res).err)) : (T)ac_res_ok((res).ok))


/* END OF RESULT DECL */



/*          *
 *  OPTION  *
 *          */
// CONFIG DEFINES:
//  -
//
// CONST DEFINES
//  -
//
// TYPES AND TYPE MACROS:
//  - Ac_OptTag
//  - Ac_OptDef(T)
//  - Ac_CharOpt
//
// FUNCTIONS AND MACROS:
//  - ac_opt_some(val)
//  - ac_opt_unwrap(opt)
//  - ac_opt_unwrap_or(opt, or)
//  - ac_opt_unwrap_or_else(opt, or_fn)
//  - ac_opt_map(T, opt, fn)
//
// USAGE:

/// The tag behind the Ac_Option
typedef enum Ac_OptTag
{
    AC_OPT_SOME,
    AC_OPT_NONE,
} Ac_OptTag;

/// Define a new Option type with an inner type of T
#define Ac_OptDef(T)   \
    struct             \
    {                  \
        Ac_OptTag tag; \
        T some;        \
    }

/// An option holding a char
typedef Ac_OptDef(char) Ac_CharOpt;

/// Construct a new opt with a some value
#define ac_opt_some(val) {.tag = AC_OPT_SOME, .some = (val)}

/// Construct a none opt value
#define ac_opt_none() {.tag = AC_OPT_NONE}

/// Unrwap an options some value. Asserts that the tag is AC_OPT_SOME
#define ac_opt_unwrap(opt)                                               \
    (ACLIB_ASSERT_FN((opt).tag == AC_OPT_SOME &&                         \
                     "Tried to unwrap option, but got tag AC_OPT_NONE"), \
     (opt).some)


/// Unwrap an option some value, or give the given or value
#define ac_opt_unwrap_or(opt, or) ((opt).tag == AC_OPT_SOME ? (opt).some : (or))

/// Unwrap an option some value, or give a value, returned from the or_fn function
#define ac_opt_unwrap_or_else(res, or_fn) ((opt).tag == AC_OPT_SOME ? (opt).tag : (or_fn)())

/// Run a function on an optipns some value, or give a none value
#define ac_opt_map(T, opt, fn) \
    ((opt).tag == AC_OPT_SOME ? (T)ac_opt_some((fn)((opt).some)) : (T)ac_opt_none())



/* END OF OPTION DECL */



/*          *
 *  VECTOR  *
 *          */
// CONFIG DEFINES:
//  - ACLIB_VEC_GROWTH
//  - ACLIB_VEC_START_CAP
//
// CONST DEFINES:
//  - AC_VEC_FOREACH
//
// TYPES AND TYPE MACROS:
//  - Ac_VecDef(T)
//
// FUNCTIONS/MACROS:
//  - ac_vec_owned_slice(vec)
//  - ac_vec_slice_range(arr, start, end)
//  - ac_vec_from(*arr, size)
//  - ac_vec_push(*vec, item)
//  - ac_vec_unshift(*vec, item)
//  - ac_vec_append(*vec, *arr, size)
//  - ac_vec_prepend(*vec, *arr, size)
//  - ac_vec_pop(*vec)
//  - ac_vec_pop_opt(T, *vec)
//  - ac_vec_shift(*vec, *out)
//      - todo: ac_vec_shift_opt(*vec, *out)
//  - ac_vec_clone_items(vec)
//      - todo: ac_vec_drain(*vec)
//      - todo: ac_vec_drain_range(*str, start, end)
//      - todo: ac_vec_remove(*vec, idx)
//      - todo: ac_vec_remove_range(*vec, start, end)
//  - ac_vec_empty(*vec)
//      - todo: ac_vec_split_at(vec, idx)
//  - ac_vec_free(vec)
//  - ac_vec_ensure_cap(vec, new_cap)
//
//
// USAGE:
//  # DEFINING
//  Define a vector type with the `VecDef(T)` macro, e.g:
//  ```c
//  typedef Ac_VecDef(int) IntVec;
//  ```
//
//  # INITIALIZING
//  To create such a vector, you can zero intialize it:
//  ```c
//  IntVec ivec = {0};
//  ```
//
//  # USING
//  After you have a vector initialized, you can manipulate it with macros. Such as `ac_vec_push()`
//  for pushing a new element
//  ```c
//  IntVec ivec = {0};
//
//  ac_vec_push(&ivec, 1); // -> ivec = { 1 }
//  ac_vec_push(&ivec, 2); // -> ivec = { 1, 2 }
//  ac_vec_push(&ivec, 3); // -> ivec = { 1, 2, 3 }
//  ```
//
//  # FREEING
//  Remember to free the vector after use with `ac_vec_free()`, to avoid memory leaks
//  ```c
//  ac_vec_free(al);
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
#define Ac_VecDef(T)              \
    struct                        \
    {                             \
        union                     \
        {                         \
            Ac_SliceDef(T) slice; \
            struct                \
            {                     \
                T* items;         \
                size_t len;       \
            };                    \
        };                        \
        size_t cap;               \
    }

/// Iterate over a vector or slice
#define AC_VEC_FOREACH(T, vec, elem) \
    for (T* elem = (vec).items; (elem) < (vec).items + (vec).len; (elem)++)

/// Create an owned slice from a vector.
/// The caller is responsible for freeing the new slice with `ac_slice_free()` after use
#define ac_vec_owned_slice(vec) ac_slice_from((ac_vec_clone_items((vec))), (vec).len)

// Slice a vector in a range from start to end.
// This does not clone or copy any of the elements of the vector, and the vector is still
// responsible for the memory
#define ac_vec_slice_range(T, arr, start, end)                           \
    (((start) < (arr).len || (end) < (arr).len)                          \
         ? ((T){.items = (arr).items + (start), .len = (end) - (start)}) \
         : ((T){0}))

/// Create a new vector from a pointer or array. This will clone the contents of the pointer or
/// array.
/// The caller is responsible for freeing the new vector with `ac_vec_free()`
#define ac_vec_from(arr, size)                                      \
    {                                                               \
        .items = __aclib_clone_arr((arr), (size) * sizeof(*(arr))), \
        .len = (size),                                              \
        .cap = (size),                                              \
    }

/// Push an item to a vector
#define ac_vec_push(vec, item)                    \
    {                                             \
        ac_vec_ensure_cap((vec), (vec)->len + 1); \
        (vec)->items[(vec)->len] = (item);        \
        (vec)->len += 1;                          \
    }


/// Add an element to the front of the vector
#define ac_vec_unshift(vec, item)                           \
    {                                                       \
        ac_vec_ensure_cap((vec), (vec)->len + 1);           \
        memcpy((vec)->items + 1, (vec)->items, (vec)->len); \
        (vec)->items[0] = (item);                           \
        (vec)->len++;                                       \
    }


/// Append a list of items to a vector
#define ac_vec_append(vec, arr, size)                  \
    {                                                  \
        ac_vec_ensure_cap((vec), (vec)->len + (size)); \
        for (size_t i = 0; i < (size); i++)            \
            (vec)->items[(vec)->len + i] = (arr)[i];   \
        (vec)->len += (size);                          \
    }

/// Prepend a list of items to the front of the vector
#define ac_vec_prepend(vec, arr, size)                                             \
    {                                                                              \
        ac_vec_ensure_cap((vec), (vec)->len + (size));                             \
        /* Move all current items to make space in front */                        \
        memmove((vec)->items + (size), (vec)->items, (vec)->len * sizeof(*(arr))); \
        /* Add the items from the arr */                                           \
        memcpy((vec)->items, (arr), (size) * sizeof(*(arr)));                      \
        (vec)->len += (size);                                                      \
    }


/// Pops and returns the last element in the vector.
/// This asserts that the vector has an element that can be popped
#define ac_vec_pop(vec)                                                                     \
    (ACLIB_ASSERT_FN((vec)->len >= 1 &&                                                     \
                     "Vector failed to pop, expected length of >= 1, but got length of 0"), \
     (vec)->len -= 1, ((vec)->items[(vec)->len]))

/// Pops and returns the last element in the vector as an option. Returns with AC_OPT_NONE if there
/// was no value to pop
#define ac_vec_pop_opt(T, vec) \
    ((vec)->len >= 1 ? ((T)ac_opt_some((vec)->items[--(vec)->len])) : (T)ac_opt_none())

// Remove the first element in the vector, and puts it into out.
/// This asserts that the vector has an element that can be shifted
#define ac_vec_shift(vec, out)                                                                   \
    {                                                                                            \
        ACLIB_ASSERT_FN((vec)->len >= 1 &&                                                       \
                        "Vector failed to shift, expected length of >= 1, but got length of 0"); \
        *(out) = (vec)->items[0];                                                                \
        memcpy((vec)->items, (vec)->items + 1, (vec)->len - 1);                                  \
        (vec)->len -= 1;                                                                         \
    }

/// Clone the items of a vector, returning them as an allocated pointer.
/// The caller is responsible for freeing the new pointer
#define ac_vec_clone_items(vec) \
    ((vec).cap > 0 ? __aclib_clone_arr((vec).items, (vec).len * sizeof((vec).items[0])) : 0)

/// Empty the given vector. This does not free or remove any memory
#define ac_vec_empty(vec) (vec)->len = 0

/// Free the given vector and its contents
#define ac_vec_free(vec)   \
    {                      \
        free((vec).items); \
        (vec).items = 0;   \
        (vec).len = 0;     \
        (vec).cap = 0;     \
    }


/// Ensure that a vector has atleast the given capacity.
#define ac_vec_ensure_cap(vec, new_cap)                                                      \
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
            ac_log(ACLIB_ERR, "Failed to reallocate vector\n");                              \
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
//  - Ac_SliceDef(T)
//
// FUNCTIONS AND MACROS:
//  - ac_slice_from(arr, size)
//  - ac_slice_free(slice)
//      - todo: ac_slice_clone(slice)
//
// USAGE:
//  # DEFINING
//  Define a slice type with the `VecDef(T)` macro, e.g:
//  ```c
//  typedef Ac_SliceDef(int) IntSlice;
//  ```
//
//  # INITIALIZING
//  To create such a vector, you can zero intialize it:
//  ```c
//  IntSlice islice = {0};
//  ```

/// Define a Slice struct with the given inner type T
#define Ac_SliceDef(T) \
    struct             \
    {                  \
        T* items;      \
        size_t len;    \
    }


/// Create a new slice from a pointer, and a len.
/// This will not clone or copy the items in the array, so it will not own the contents.
/// Do not use `ac_slice_free()` on a slice from this, unless you know what you are doing, as that
/// would free the original contents.
#define ac_slice_from(arr, size) \
    {                            \
        .items = arr,            \
        .len = size,             \
    }


/// Free a slice. Be sure that the slice owns its contents before calling this, as it otherwise can
/// create memory issues
#define ac_slice_free(slice)  \
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
//  - AC_STR_FMT
//  - AC_STR_ARG
//
// TYPES AND TYPE MACROS:
//  - Ac_StrSlice
//  - Ac_String
//  - Ac_StrVec
//
// FUNCTIONS AND MACROS:
//  - ac_str_slice_with_len(len)
//  - ac_str_slice_from(chs)
//  - ac_str_owned_slice(str)
//  - ac_str_slice_clone(slice)
//  - ac_str_slice_range(str, start, end)
//  - ac_str_slice_free(*slice)
//
//  - ac_str_with_capacity(capacity)
//  - ac_str_from(*chs)
//  - ac_str_push(*str, ch)
//  - ac_str_unshift(*str, ch)
//  - ac_str_append(*str, *chs)
//  - ac_str_appendf(*str, *fmt, ...)
//  - ac_str_prepend(*str, *chs)
//  - ac_str_prependf(*str, *chs)
//  - ac_str_pop(*str)
//  - ac_str_pop_opt(*str)
//  - ac_str_shift(*str)
//  - ac_str_shift_opt(*str)
//  - ac_str_clone_chars(str)
//  - ac_str_drain(*str);
//  - ac_str_drain_range(*str, start, end);
//  - ac_str_remove(*str, idx)
//  - ac_str_remove_range(*str, start, end)
//  - ac_str_empty(*str)
//  - ac_str_free(*str)
//  - ac_str_ensure_cap(*str, new_cap)
//  - ac_str_trim_front(*str)
//  - ac_str_trim_back(*str)
//  - ac_str_trim(*str)
//  - ac_str_split_by(str, delim)
//  - ac_str_split_by_once(str, delim)
//  - ac_str_split_by_many(str, *delims)
//  - ac_str_split_at(str, idx)
//  - ac_str_read_file(*buffer, *file)
//  - ac_str_read_lines(*linebuffer, *file)
//
//  - ac_str_trimmed_front(slice)
//  - ac_str_trimmed_back(slice)
//  - ac_str_trimmed(slice)
//
// USAGE:
//  # DEFINING
//  Define a slice type with the `VecDef(T)` macro, e.g:
//  ```c
//  typedef Ac_SliceDef(int) IntVec;
//  ```
//
//  # INITIALIZING
//  To create such a vector, you can zero intialize it:
//  ```c
//  IntVec ivec = {0};
//  ```
//
//  # USING
//  After you have a vector initialized, you can manipulate it with macros. Such as `ac_vec_push()`
//  for pushing a new element
//  ```c
//  IntVec ivec = {0};
//
//  ac_vec_push(&ivec, 1); // -> ivec = { 1 }
//  ac_vec_push(&ivec, 2); // -> ivec = { 1, 2 }
//  ac_vec_push(&ivec, 3); // -> ivec = { 1, 2, 3 }
//  ```
//
//  # FREEING
//  Remember to free the vector after use with `ac_vec_free()`, to avoid memory leaks
//  ```c
//  ac_vec_free(al);
//  ```

/// A string slice, which holds a cstr and a length
typedef struct Ac_StrSlice
{
    /// The characters of the string slice. Can be used as a cstr
    char* chars;
    /// The length of the string slice
    size_t len;
} Ac_StrSlice;


/// A dynamic and owned string
typedef struct Ac_String
{
    union
    {
        /// The string of the slice
        Ac_StrSlice slice;
        struct
        {
            /// The characters of the string. Can be used as a cstr
            char* chars;
            /// The length of the string
            size_t len;
        };
    };
    /// The capacity of the string
    size_t cap;
} Ac_String;


/// A format string used to print strings and string slices properly. Remember to use `AC_STR_ARG()`
/// as the format argument.
#define AC_STR_FMT "%.*s"
/// The arg used
#define AC_STR_ARG(str) (int)(str).len, (str).chars

/// A vector containing string slices.
/// To avoid leaking memory, remember to free each individual string slice by themselves, as
/// `vec_free` will not do this for you.
typedef Ac_VecDef(Ac_StrSlice) Ac_StrVec;

/// Allocate a new empty string slice with a specific length. The caller is responsible for freeing
/// the memory with `ac_slice_free()`
ACLIBDEF Ac_StrSlice ac_str_slice_with_len(size_t len);

/// Get a slice to a c string, this will not copy or clone the chars, just point to them.
ACLIBDEF Ac_StrSlice ac_str_slice_from(char* chs);

/// Get an owned slice to the contents of a string. The caller is responsible for freeing the new
/// slice with `ac_str_slice_free(slice)`
ACLIBDEF Ac_StrSlice ac_str_owned_slice(Ac_String str);

/// Clone a string slice and its chars.
/// The caller i responsible for freeing the slice with `ac_str_slice_free()`
ACLIBDEF Ac_StrSlice ac_str_slice_clone(Ac_StrSlice slice);

/// Get a slice from a string in a specific range. This does not copy or clone the chars in the
/// range.
ACLIBDEF Ac_StrSlice ac_str_slice_range(Ac_String str, size_t start, size_t end);

/// Free a string slice and its memory. This should *only* be called if the slice owns its own
/// characters
ACLIBDEF void ac_str_slice_free(Ac_StrSlice* slice);

/// Allocates a new string with a specific capacity. The caller is responsible for freeing the
/// string with `ac_str_free()`
ACLIBDEF Ac_String ac_str_with_capacity(size_t capacity);

/// Create a new string from a cstr. This will clone the contents of the cstr.
/// The caller is responsible for freeing the new string with `ac_str_free()`
ACLIBDEF Ac_String ac_str_from(char* chs);

/// Push a char unto the end of a string
ACLIBDEF void ac_str_push(Ac_String* str, char ch);

/// Adds a char to the start of a string
ACLIBDEF void ac_str_unshift(Ac_String* str, char ch);

/// Append a cstr unto the end of a string
ACLIBDEF void ac_str_append(Ac_String* str, char* chs);

/// Append a formatted cstr unto the end of a string
ACLIBDEF void ac_str_appendf(Ac_String* str, const char* fmt, ...);

/// Prepend a cstr to the front of a string
ACLIBDEF void ac_str_prepend(Ac_String* str, char* chs);

/// Prepend a formatted cstr to the front of a string
ACLIBDEF void ac_str_prependf(Ac_String* str, const char* fmt, ...);

/// Pop and return the last char of a string.
/// This asserts that the string has a char that can be popped
ACLIBDEF char ac_str_pop(Ac_String* str);

/// Pop and return the last char of a string as an Option, returning AC_OPT_NONE if there are no
/// chars to pop
ACLIBDEF Ac_CharOpt ac_str_pop_opt(Ac_String* str);

/// Remove and return the first char of a string.
/// This asserts that the string has a char that can be popped
ACLIBDEF char ac_str_shift(Ac_String* str);

/// Remove and return the first char of a string as an Option, returning AC_OPT_NONE if there are no
/// chars to remove
ACLIBDEF Ac_CharOpt ac_str_shift_opt(Ac_String* str);

/// Clone the chars behind a string, and return them as an owned string slice.
/// The caller is responsible for freeing the allocated cstr
ACLIBDEF char* ac_str_clone_chars(Ac_String str);

/// Puts all the contents of a string into a slice, removing all of the contents of the original
/// string.
/// The slice is now the owner of the memory.
/// The caller is responsible for freeing the string slice with `ac_str_slice_free()`
ACLIBDEF Ac_StrSlice ac_str_drain(Ac_String* str);

/// Puts all the contents of a string in a given range into a slice, removing the elements from the
/// original string. The slice is now the owner of the removed memory. The caller is responsible for
/// freeing the string slice with `ac_str_slice_free()`
ACLIBDEF Ac_StrSlice ac_str_drain_range(Ac_String* str, size_t start, size_t end);

/// Remove a char at a given index from a string
ACLIBDEF void ac_str_remove(Ac_String* str, size_t idx);

/// Remove a set of chars in a given range, from a string
ACLIBDEF void ac_str_remove_range(Ac_String* str, size_t start, size_t end);

/// Empty the given string. This does not free or remove any memory
ACLIBDEF void ac_str_empty(Ac_String* str);

/// Free the given vector and its contents
ACLIBDEF void ac_str_free(Ac_String* str);

/// Ensure that a string has atleast the given capacity.
ACLIBDEF void ac_str_ensure_cap(Ac_String* str, size_t new_cap);

/// Trim all whitespace from the front of a string
ACLIBDEF void ac_str_trim_front(Ac_String* str);

/// Trim all whitespace from the back of the string
ACLIBDEF void ac_str_trim_back(Ac_String* str);

/// Trim all whitespace from the front and back of a string
ACLIBDEF void ac_str_trim(Ac_String* str);

/// Split a string by a delimeter
ACLIBDEF Ac_StrVec ac_str_split_by(Ac_String str, char delim);

/// Split a string by a set of delimeters
ACLIBDEF Ac_StrVec ac_str_split_by_many(Ac_String str, char* delims);

/// Split a string by a delimeter once
ACLIBDEF Ac_StrVec ac_str_split_by_once(Ac_String str, char delim);

/// Split a string at an index
ACLIBDEF Ac_StrVec ac_str_split_at(Ac_String str, size_t idx);

/// Reads a file into a string buffer, and returns the amount of bytes read.
ACLIBDEF size_t ac_str_read_file(Ac_String* buffer, FILE* file);

/// Reads all the lines in a file into a line buffer, and returns the amount of bytes read.
ACLIBDEF size_t ac_str_read_lines(Ac_StrVec* linebuffer, FILE* file);

/// Trim the front of a string slice, without allocating or copying any bytes. The returned slice is
/// just a pointer to the original slice.
ACLIBDEF Ac_StrSlice ac_str_trimmed_front(Ac_StrSlice slice);

/// Trim the back of a string slice, without allocating or copying any bytes. The returned slice is
/// just a pointer to the original slice.
ACLIBDEF Ac_StrSlice ac_str_trimmed_back(Ac_StrSlice slice);

/// Trim a string slice, without allocating or copying any bytes. The returned slice is just a
/// pointer to the original slice.
ACLIBDEF Ac_StrSlice ac_str_trimmed(Ac_StrSlice slice);

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
//  - ac_ascii_is_alphabetic(ch)
//  - ac_ascii_is_numeric(ch)
//  - ac_ascii_is_alphanumeric(ch)
//  - ac_ascii_is_whitespace(ch)
//  - ac_ascii_is_uppercase(ch)
//  - ac_ascii_is_lowercase(ch)
//  - ac_ascii_to_uppercase(ch)
//  - ac_ascii_to_lowercase(ch)
//
// USAGE:

/// Checks if an ascii char is alphabetic, i.e. [a-zA-Z]
char ac_ascii_is_alphabetic(char ch);
/// Checks if an ascii char is numeric, i.e. [0-9]
bool ac_ascii_is_numeric(char ch);
/// Checks if an ascii char is alphanumeric, i.e. [a-zA-Z0-9]
bool ac_ascii_is_alphanumeric(char ch);
/// Checks if an ascii char is whitespace, e.g. '\n', '\t', ' '
bool ac_ascii_is_whitespace(char ch);
/// Checks if an ascii char is uppercase
bool ac_ascii_is_uppercase(char ch);
/// Checks if an ascii char is lowercase
bool ac_ascii_is_lowercase(char ch);
/// Returns the uppercase version of an ascii char
char ac_ascii_to_uppercase(char ch);
/// Returns the lowercase version of an ascii char
char ac_ascii_to_lowercase(char ch);

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
//  - Ac_LogLevel
//
// FUNCTIONS AND MACROS:
//  - ac_log(loglvl, fmt, ...)
//  - ac_todo(msg)
//
// USAGE:

#ifndef ACLIB_LOG_FN
/// Sets the function that `ac_log` will write to. The function must have the signature
/// `void (Ac_LogLevel loglvl, const char* fmt, ...)`
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

/// The loglevel used by ac_log()
typedef enum Ac_LogLevel
{
    ACLIB_DEBUG,
    ACLIB_INFO,
    ACLIB_WARN,
    ACLIB_SUCCESS,
    ACLIB_ERR,
    ACLIB_NO_LOGS,
} Ac_LogLevel;

/// The current minimun log level for `ac_log`
extern Ac_LogLevel aclib_log_level;
/// the file descriptor that aclibs logging default implementation will write to
extern FILE* aclib_log_fd;
/// Sets the function that `ac_log` will write to. The function must have the signature
/// `void (Ac_LogLevel loglvl, const char* fmt, ...)`
extern void (*aclib_log_fn_ptr)(Ac_LogLevel, const char*, ...);

/// The default logging function for aclib
ACLIBDEF void __aclib_default_log_fn(Ac_LogLevel loglvl, const char* fmt, ...);
/// Print a log messae
#define ac_log (aclib_log_fd = (!aclib_log_fd) ? stderr : aclib_log_fd, *aclib_log_fn_ptr)

#ifndef ACLIB_SILENT
#define __ac_intern_log(...) ac_log(__VA_ARGS__)
#else
#define __ac_intern_log(...)
#endif

#define ac_todo(msg) (ac_log(ACLIB_ERR, "%s:%d: TODO: %s\n", __FILE__, __LINE__, (msg)), abort())

/* END OF LOGGING DECL */



/*                        *
 *  ACLIB IMPLEMENTATION  *
 *                        */
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

ACLIBDEF Ac_StrSlice ac_str_slice_with_len(size_t len)
{
    return (Ac_StrSlice){
        .chars = (char*)ACLIB_CALLOC_FN(len + 1, sizeof(char)),
        .len = len,
    };
}

ACLIBDEF Ac_StrSlice ac_str_slice_from(char* chs)
{
    return (Ac_StrSlice){
        .chars = chs,
        .len = strlen(chs),
    };
}

ACLIBDEF Ac_StrSlice ac_str_owned_slice(Ac_String str)
{
    if (str.cap == 0)
        return (Ac_StrSlice){0};

    char* buffer = ac_str_clone_chars(str);
    return ac_str_slice_from(buffer);
}

ACLIBDEF Ac_StrSlice ac_str_slice_clone(Ac_StrSlice slice)
{
    char* chars = (char*)ACLIB_MALLOC_FN((slice.len + 1) * sizeof(char));
    memcpy(chars, slice.chars, slice.len * sizeof(char));

    chars[slice.len] = '\0';
    return (Ac_StrSlice){.chars = chars, .len = slice.len};
}

ACLIBDEF Ac_StrSlice ac_str_slice_range(Ac_String str, size_t start, size_t end)
{
    if (start >= str.len || end > str.len || start >= end)
        return (Ac_StrSlice){0};

    return (Ac_StrSlice){
        .chars = str.chars + start,
        .len = end - start,
    };
}

ACLIBDEF void ac_str_slice_free(Ac_StrSlice* slice)
{
    free(slice->chars);
    slice->len = 0;
}

ACLIBDEF Ac_String ac_str_with_capacity(size_t capacity)
{
    return (Ac_String){
        .chars = (char*)ACLIB_CALLOC_FN(capacity + 1, sizeof(char)),
        .len = 0,
        .cap = capacity,
    };
}

ACLIBDEF Ac_String ac_str_from(char* chs)
{
    Ac_String str = {0};
    ac_str_append(&str, chs);
    return str;
}

ACLIBDEF void ac_str_push(Ac_String* str, char ch)
{
    ac_str_ensure_cap(str, str->len + 1);

    str->chars[str->len] = ch;
    str->chars[str->len + 1] = '\0';
    str->len += 1;
}

ACLIBDEF void ac_str_unshift(Ac_String* str, char ch)
{
    ac_str_ensure_cap(str, str->len + 1);
    memmove(str->chars + 1, str->chars, str->len * sizeof(char));
    str->chars[0] = ch;
    str->len += 1;
    str->chars[str->len] = '\0';
}

ACLIBDEF void ac_str_append(Ac_String* str, char* chs)
{
    size_t size = strlen(chs);
    ac_str_ensure_cap(str, str->len + size);

    for (size_t i = 0; i < size; i++)
        str->chars[str->len + i] = chs[i];

    str->chars[str->len + size] = '\0';
    str->len += size;
}

ACLIBDEF void ac_str_appendf(Ac_String* str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t needed = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);

    ac_str_ensure_cap(str, str->len + needed);

    char* dest = str->chars + str->len;
    va_start(args, fmt);
    vsprintf(dest, fmt, args);
    va_end(args);

    // sprintf automatically adds '\0', so take account for it
    str->len += needed - 1;
}

ACLIBDEF void ac_str_prepend(Ac_String* str, char* chs)
{
    size_t size = strlen(chs);
    ac_str_ensure_cap(str, str->len + size);

    memcpy(str->chars + size, str->chars, str->len * sizeof(char));
    memcpy(str->chars, chs, size * sizeof(char));

    str->chars[str->len + size] = '\0';
    str->len += size;
}

ACLIBDEF void ac_str_prependf(Ac_String* str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t needed = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);

    ac_str_ensure_cap(str, str->len + needed);

    memcpy(str->chars + needed - 1, str->chars, str->len * sizeof(char));

    Ac_StrSlice format_buf = ac_str_slice_with_len(needed - 1);
    va_start(args, fmt);
    vsprintf(format_buf.chars, fmt, args);
    va_end(args);

    memcpy(str->chars, format_buf.chars, format_buf.len * sizeof(char));

    ac_str_slice_free(&format_buf);

    // sprintf automatically adds '\0', so take account for it
    str->len += needed - 1;
}

ACLIBDEF char ac_str_pop(Ac_String* str)
{
    ACLIB_ASSERT_FN(str->len >= 1 &&
                    "String failed to pop, expected length of >= 1, but got length of 0");
    str->len -= 1;
    char popped = str->chars[str->len];
    str->chars[str->len] = '\0';
    return popped;
}

ACLIBDEF Ac_CharOpt ac_str_pop_opt(Ac_String* str)
{
    if (str->len == 0)
        return (Ac_CharOpt)ac_opt_none();

    return (Ac_CharOpt)ac_opt_some(ac_str_pop(str));
}

ACLIBDEF char ac_str_shift(Ac_String* str)
{
    ACLIB_ASSERT_FN(str->len >= 1 &&
                    "String failed to shift, expected length of >= 1, but got length of 0");

    char shifted = str->chars[0];

    memcpy(str->chars, str->chars + 1, (str->len - 1) * sizeof(char));
    str->len -= 1;
    str->chars[str->len] = '\0';

    return shifted;
}

ACLIBDEF Ac_CharOpt ac_str_shift_opt(Ac_String* str)
{
    if (str->len == 0)
        return (Ac_CharOpt)ac_opt_none();

    return (Ac_CharOpt)ac_opt_some(ac_str_shift(str));
}

ACLIBDEF char* ac_str_clone_chars(Ac_String str)
{
    char* buffer = (char*)malloc((str.len + 1) * sizeof(char));

    if (buffer == NULL)
    {
        ac_log(ACLIB_ERR, "Failed to clone string chars\n");
        exit(EXIT_FAILURE);
    }

    memcpy(buffer, str.chars, str.len * sizeof(char));
    buffer[str.len] = '\0';

    return buffer;
}

ACLIBDEF Ac_StrSlice ac_str_drain(Ac_String* str)
{
    Ac_StrSlice slice = ac_str_slice_from(str->chars);

    *str = (Ac_String){0};

    return slice;
}

ACLIBDEF Ac_StrSlice ac_str_drain_range(Ac_String* str, size_t start, size_t end)
{
    Ac_StrSlice slice = ac_str_slice_range(*str, start, end);
    slice = ac_str_slice_clone(slice);

    ac_str_remove_range(str, start, end);

    return slice;
}

ACLIBDEF void ac_str_remove(Ac_String* str, size_t idx)
{
    if (idx >= str->len)
        return;

    ac_str_remove_range(str, idx, idx + 1);
}

ACLIBDEF void ac_str_remove_range(Ac_String* str, size_t start, size_t end)
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

ACLIBDEF void ac_str_empty(Ac_String* str)
{
    str->len = 0;
    if (str->chars)
        str->chars[0] = '\0';
}

ACLIBDEF void ac_str_free(Ac_String* str)
{
    if (str->chars == NULL)
        return;

    free(str->chars);
    str->chars = (char*)0;
    str->len = 0;
    str->cap = 0;
}

ACLIBDEF void ac_str_ensure_cap(Ac_String* str, size_t new_cap)
{
    if (str->cap < new_cap + 1)
    {
        // + 1 for the '\0'
        str->chars = (char*)ACLIB_REALLOC_FN(str->chars, ((new_cap * ACLIB_VEC_GROWTH_RATE) + 1) *
                                                             sizeof(char));
        str->cap = new_cap;

        if (str->chars == NULL)
        {
            ac_log(ACLIB_ERR, "Failed to reallocate string\n");
            exit(EXIT_FAILURE);
        }

        str->chars[str->cap] = '\0';
    }
}


#define INTERNAC_BUF_SIZE 256
ACLIBDEF size_t ac_str_read_file(Ac_String* buffer, FILE* file)
{
    size_t bytes_read = 0;
    char char_buf[INTERNAC_BUF_SIZE];
    while (fgets(char_buf, INTERNAC_BUF_SIZE, file))
    {
        bytes_read += strlen(char_buf);
        ac_str_append(buffer, char_buf);
    }

    return bytes_read;
}

ACLIBDEF size_t ac_str_read_lines(Ac_StrVec* linebuffer, FILE* file)
{
    size_t bytes_read = 0;

    char char_buf[INTERNAC_BUF_SIZE];
    while (fgets(char_buf, INTERNAC_BUF_SIZE, file))
    {
        bytes_read += strlen(char_buf);

        Ac_String line = ac_str_from(char_buf);
        size_t old_size = strlen(char_buf);
        while (char_buf[strlen(char_buf) - 1] != '\n')
        {
            fgets(char_buf, INTERNAC_BUF_SIZE, file);
            if (strlen(char_buf) == old_size)
                break;
            ac_str_append(&line, char_buf);
        }

        // pop the '\n'
        if (line.chars[line.len - 1] == '\n')
            ac_str_pop(&line);

        ac_vec_push(linebuffer, ac_str_owned_slice(line));
        ac_str_free(&line);
    }

    return bytes_read;
}
#undef INTERNAC_BUF_SIZE

ACLIBDEF void ac_str_trim_front(Ac_String* str)
{
    if (str->len == 0)
        return;

    size_t end = 0;
    while (end < str->len && ac_ascii_is_whitespace(str->chars[end]))
        end++;

    ac_str_remove_range(str, 0, end);
}

ACLIBDEF void ac_str_trim_back(Ac_String* str)
{
    if (str->len == 0)
        return;

    size_t start = str->len;
    while (start > 0 && ac_ascii_is_whitespace(str->chars[start - 1]))
        start--;

    ac_str_remove_range(str, start, str->len);
}

ACLIBDEF void ac_str_trim(Ac_String* str)
{
    ac_str_trim_back(str);
    ac_str_trim_front(str);
}

ACLIBDEF Ac_StrSlice ac_str_trimmed_front(Ac_StrSlice slice)
{
    if (slice.len == 0)
        return (Ac_StrSlice){0};

    size_t end = 0;
    while (end < slice.len && ac_ascii_is_whitespace(slice.chars[end]))
        end++;

    return (Ac_StrSlice){.chars = slice.chars + end, .len = slice.len - end};
}

ACLIBDEF Ac_StrSlice ac_str_trimmed_back(Ac_StrSlice slice)
{
    if (slice.len == 0)
        return (Ac_StrSlice){0};

    size_t start = slice.len;
    while (start > 0 && ac_ascii_is_whitespace(slice.chars[start - 1]))
        start--;

    return (Ac_StrSlice){.chars = slice.chars, .len = start};
}

ACLIBDEF Ac_StrSlice ac_str_trimmed(Ac_StrSlice slice)
{
    Ac_StrSlice trimmed_front = ac_str_trimmed_front(slice);
    return ac_str_trimmed_back(trimmed_front);
}

ACLIBDEF Ac_StrVec ac_str_split_by(Ac_String str, char delim)
{
    size_t start = 0;
    size_t end = 0;

    Ac_StrVec parts = {0};

    while (end <= str.len)
    {
        if (str.chars[end] == delim)
        {
            Ac_StrSlice slice = ac_str_slice_range(str, start, end);
            slice = ac_str_slice_clone(slice);

            ac_vec_push(&parts, slice);
            end++;
            start = end;
        }
        end++;
    }

    Ac_StrSlice slice = ac_str_slice_range(str, start, str.len);
    slice = ac_str_slice_clone(slice);

    ac_vec_push(&parts, slice);

    return parts;
}

/// Split a string by a set of delimeters
ACLIBDEF Ac_StrVec ac_str_split_by_many(Ac_String str, char* delims)
{
    size_t start = 0;
    size_t end = 0;

    Ac_StrVec parts = {0};

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
            Ac_StrSlice slice = ac_str_slice_range(str, start, end);
            slice = ac_str_slice_clone(slice);

            ac_vec_push(&parts, slice);
            end++;
            start = end;
        }
        end++;
    }

    Ac_StrSlice slice = ac_str_slice_range(str, start, str.len);
    slice = ac_str_slice_clone(slice);

    ac_vec_push(&parts, slice);

    return parts;
}

/// Split a string by a delimeter once
ACLIBDEF Ac_StrVec ac_str_split_by_once(Ac_String str, char delim)
{
    size_t end = 0;

    Ac_StrVec parts = {0};

    while (end < str.len)
    {
        if (str.chars[end] == delim)
        {
            Ac_StrSlice slice = ac_str_slice_range(str, 0, end);
            slice = ac_str_slice_clone(slice);

            ac_vec_push(&parts, slice);
            break;
        }
        end++;
    }

    Ac_StrSlice rest = ac_str_slice_range(str, end + 1, str.len);
    rest = ac_str_slice_clone(rest);
    ac_vec_push(&parts, rest);

    return parts;
}

/// Split a string at an index
ACLIBDEF Ac_StrVec ac_str_split_at(Ac_String str, size_t idx)
{
    Ac_StrVec parts = {0};

    if (idx >= str.len)
    {
        ac_vec_push(&parts, ac_str_owned_slice(str));
        return parts;
    }

    Ac_StrSlice first_part = ac_str_slice_range(str, 0, idx);
    first_part = ac_str_slice_clone(first_part);
    ac_vec_push(&parts, first_part);

    Ac_StrSlice second_part = ac_str_slice_range(str, idx, str.len);
    second_part = ac_str_slice_clone(second_part);
    ac_vec_push(&parts, second_part);

    return parts;
}

/* END OF STRING IMPLEMENTATION */



/*                        *
 *  ASCII IMPLEMENTATION  *
 *                        */

char ac_ascii_is_alphabetic(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool ac_ascii_is_numeric(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool ac_ascii_is_alphanumeric(char ch)
{
    return ac_ascii_is_alphabetic(ch) || ac_ascii_is_numeric(ch);
}

bool ac_ascii_is_whitespace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f';
}

bool ac_ascii_is_uppercase(char ch)
{
    return ch >= 'A' && ch <= 'Z';
}

bool ac_ascii_is_lowercase(char ch)
{
    return ch >= 'a' && ch <= 'z';
}

char ac_ascii_to_uppercase(char ch)
{
    if (!ac_ascii_is_alphabetic(ch))
        return ch;

    // e.g. convert 1000010 (B) to 1100010 (b)
    return ch & ~0x20;
}

char ac_ascii_to_lowercase(char ch)
{
    if (!ac_ascii_is_alphabetic(ch))
        return ch;

    // e.g. convert 1000010 (B) to 1100010 (b)
    return ch | 0x20;
}

/* END OF ASCII IMPLEMENTATION */



/*                          *
 *  LOGGING IMPLEMENTATION  *
 *                          */

Ac_LogLevel aclib_log_level = ACLIB_INFO;
FILE* aclib_log_fd = NULL;
void (*aclib_log_fn_ptr)(Ac_LogLevel, const char*, ...) = __aclib_default_log_fn;

ACLIBDEF void __aclib_default_log_fn(Ac_LogLevel loglvl, const char* fmt, ...)
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



/*                      *
 *  ACLIB STRIP PREFIX  *
 *                      */
#ifdef ACLIB_STRIP_PREFIX



/*                       *
 *  RESULT STRIP PREFIX  *
 *                       */

#define ResTag Ac_ResTag
#define ResDef Ac_ResDef
#define res_ok ac_res_ok
#define res_err ac_res_err
#define res_try ac_res_try
#define res_unwrap ac_res_unwrap
#define res_unwrap_err ac_res_unwrap_err
#define res_unwrap_or ac_res_unwrap_or
#define res_unwrap_or_else ac_res_unwrap_or_else
#define res_map ac_res_map
#define res_map_err ac_res_map_err

/* END OF RESULT STRIP PREFIX */



/*                       *
 *  OPTION STRIP PREFIX  *
 *                       */

#define OptTag Ac_OptTag
#define OptDef Ac_OptDef
#define CharOpt Ac_CharOpt
#define opt_some ac_opt_some
#define opt_unwrap ac_opt_unwrap
#define opt_unwrap_or ac_opt_unwrap_or
#define opt_unwrap_or_else ac_opt_unwrap_or_else
#define opt_map ac_opt_map

/* END OF RESULT STRIP PREFIX */

/*                      *
 *  VECTOR STRIP PREFIX  *
 *                      */

#define VEC_FOREACH AC_VEC_FOREACH

#define VecDef Ac_VecDef

#define vec_owned_slice ac_vec_owned_slice
#define vec_slice_range ac_vec_slice_range
#define vec_from ac_vec_from
#define vec_push ac_vec_push
#define vec_unshift ac_vec_unshift
#define vec_append ac_vec_append
#define vec_prepend ac_vec_prepend
#define vec_pop ac_vec_pop
#define vec_pop_opt ac_vec_pop_opt
#define vec_shift ac_vec_shift
#define vec_clone_items ac_vec_clone_items
#define vec_empty ac_vec_empty
#define vec_free ac_vec_free
#define vec_ensure_cap ac_vec_ensure_cap

/* END OF VECTOR STRIP PREFIX */



/*                      *
 *  SLICE STRIP PREFIX  *
 *                      */

#define SliceDef Ac_SliceDef

#define slice_from ac_slice_from
#define slice_free ac_slice_free

/* END OF SLICE STRIP PREFIX */



/*                       *
 *  STRING STRIP PREFIX  *
 *                       */

#define STR_FMT AC_STR_FMT
#define STR_ARG AC_STR_ARG

#define String Ac_String
#define StrSlice Ac_StrSlice
#define StrVec Ac_StrVec

#define str_slice_with_len ac_str_slice_with_len
#define str_slice_from ac_str_slice_from
#define str_owned_slice ac_str_owned_slice
#define str_slice_clone ac_str_slice_clone
#define str_slice_range ac_str_slice_range
#define str_slice_free ac_str_slice_free

#define str_with_capacity ac_str_with_capacity
#define str_from ac_str_from
#define str_push ac_str_push
#define str_unshift ac_str_unshift
#define str_append ac_str_append
#define str_appendf ac_str_appendf
#define str_prepend ac_str_prepend
#define str_prependf ac_str_prependf
#define str_pop ac_str_pop
#define str_pop_opt ac_str_pop_opt
#define str_shift ac_str_shift
#define str_shift_opt ac_str_shift_opt
#define str_clone_chars ac_str_clone_chars
#define str_drain ac_str_drain
#define str_drain_range ac_str_drain_range
#define str_remove ac_str_remove
#define str_remove_range ac_str_remove_range
#define str_empty ac_str_empty
#define str_free ac_str_free
#define str_ensure_cap ac_str_ensure_cap
#define str_trim_front ac_str_trim_front
#define str_trim_back ac_str_trim_back
#define str_trim ac_str_trim
#define str_split_by ac_str_split_by
#define str_split_by_once ac_str_split_by_once
#define str_split_by_many ac_str_split_by_many
#define str_split_at ac_str_split_at
#define str_read_file ac_str_read_file
#define str_read_lines ac_str_read_lines

#define str_trimmed_front ac_str_trimmed_front
#define str_trimmed_back ac_str_trimmed_back
#define str_trimmed ac_str_trimmed

/* END OF STRING STRIP PREFIX */



/*                      *
 *  ASCII STRIP PREFIX  *
 *                      */

#define ascii_is_alphabetic ac_ascii_is_alphabetic
#define ascii_is_numeric ac_ascii_is_numeric
#define ascii_is_alphanumeric ac_ascii_is_alphanumeric
#define ascii_is_whitespace ac_ascii_is_whitespace
#define ascii_is_uppercase ac_ascii_is_uppercase
#define ascii_is_lowercase ac_ascii_is_lowercase
#define ascii_to_uppercase ac_ascii_to_uppercase
#define ascii_to_lowercase ac_ascii_to_lowercase

/* END OF ASCII STRIP PREFIX */



/*                        *
 *  LOGGING STRIP PREFIX  *
 *                        */

#define LogLevel Ac_LogLevel
#define log ac_log
#define todo ac_todo

/* END OF LOGGING STRIP PREFIX */



#endif // ACLIB_STRIP_PREFIX


#endif // __ACLIB_H
