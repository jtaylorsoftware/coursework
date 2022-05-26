/**
 * Generates a course plan from an offerings file, prerequisites file,
 * and required courses file.
 *
 * Usage: Lab1 <offerings> <prerequisites> <required>
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define TRUE 1
#define FALSE 0 

#define OFFERING_FILE 1
#define PREREQ_FILE 2
#define REQ_FILE 3

#define MAX_COURSENAME_SIZE 20

typedef unsigned char bool;

#define function_signature(ret_type, name, ...) ret_type name (__VA_ARGS__)

#define define_func_alias(ret_type, name, pfunc, ...)\
typedef ret_type (*name##_func)(__VA_ARGS__);\
static const name##_func name = pfunc;\

#define USE_DEFAULT_ALLOC(type)\
function_signature(type, alloc_##type, void){ type t = {0}; return t;}

#define USE_AS_ALLOC(type, func)\
define_func_alias(type, alloc_##type, func, void)\

#define USE_DEFAULT_HASH(type)\
function_signature(unsigned long, hash_##type, type t){ return rand(); }\

#define USE_AS_HASH(type, func)\
define_func_alias(unsigned long, hash_##type, func, type)\

#define USE_DEFAULT_COPY(type)\
function_signature(type, copy_##type, type v){ type cpy = v; return cpy; }\

#define USE_AS_COPY(type, func)\
define_func_alias(type, copy_##type, func, type)\

#define USE_DEFAULT_SWAP(type)\
function_signature(void, swap_##type, type* lhs, type*rhs){ type t = *lhs; *lhs = *rhs; *rhs = t; }\

#define USE_AS_SWAP(type, func)\
define_func_alias(void, swap_##type,func, type* lhs, type* rhs)

#define USE_DEFAULT_EQUALS(type)\
function_signature(bool, equals_##type, type lhs, type rhs){ return lhs == rhs; }\

#define USE_AS_EQUALS(type, func)\
define_func_alias(bool, equals_##type, func, type, type)\

#define USE_DEFAULT_FREE(type)\
function_signature(void, free_##type, type t){}\

#define USE_AS_FREE(type, func)\
define_func_alias(void, free_##type, func, type)\

/* Makes writing out Pair types easier */
#define CONCAT(a, b) a##_##b
#define EVAL(a, b) CONCAT(a, b)
#define PAIR(a, b) EVAL(Pair, CONCAT(a, b))
#define PAIR_FUNC_NAME(name, a, b) EVAL(name, PAIR(a, b))
#define PAIR_FUNC_SIGNATURE(ret, name, a, b, ...) function_signature(ret, PAIR_FUNC_NAME(name, a, b), __VA_ARGS__)

#define PAIR_FUNC(name, t,u, ...) EVAL(name, EVAL(Pair, EVAL(t, u)))(__VA_ARGS__)

#define DEFINE_PAIR(t, u)\
typedef struct PAIR(t, u) PAIR(t, u);\
struct PAIR(t, u)\
{\
    t first;\
    u second;\
};\
\
PAIR_FUNC_SIGNATURE(PAIR(t,u), Alloc, t, u, void)\
{\
    PAIR(t, u) p = {0};\
    p.first = alloc_##t();\
    p.second = alloc_##u();\
    return p;\
}\
PAIR_FUNC_SIGNATURE(PAIR(t, u), Create, t, u, t first, u second)\
{\
    PAIR(t, u) p = {0};\
    p.first = copy_##t(first);\
    p.second = copy_##u(second);\
    return p;\
}\
PAIR_FUNC_SIGNATURE(void, Destroy, t, u, PAIR(t, u) p)\
{\
    free_##t(p.first);\
    free_##u(p.second);\
}\
PAIR_FUNC_SIGNATURE(PAIR(t, u), Copy, t, u, PAIR(t,u) q)\
{\
    PAIR(t, u) p = {0};\
    p.first = copy_##t(q.first);\
    p.second = copy_##u(q.second);\
    return p;\
}\
PAIR_FUNC_SIGNATURE( bool, Equals, t, u, PAIR( t, u ) e, PAIR( t, u ) v )\
{\
    if ( equals_##t(e.first, v.first) == TRUE && equals_##u(e.second, v.second) == TRUE)\
        return TRUE;\
    \
    return FALSE;\
}\

#define LN(t) EVAL(LN, t)
#define LN_FUNC_NAME(name, a) EVAL(name, LN(a))
#define LN_FUNC_SIGNATURE(ret, name, a, ...) function_signature(ret, LN_FUNC_NAME(name, a), __VA_ARGS__)
#define LN_FUNC(name, t, ...) EVAL(name, EVAL(LN, t))(__VA_ARGS__)

#define DEFINE_LN(type)\
typedef struct LN(type) LN(type);\
struct LN(type)\
{\
    type data;\
    struct LN(type)* next;\
};\
\
LN_FUNC_SIGNATURE(LN(type)*, Create, type, type data, LN(type)* next)\
{\
    LN(type)* ln = malloc( sizeof( struct LN(type) ) );\
    ln->data = copy_##type(data);\
    ln->next = next;\
    return ln;\
}\
\
LN_FUNC_SIGNATURE(void, Destroy, type, LN(type)* ln)\
{\
    if ( ln )\
    {\
        free_##type( ln->data );\
        free( ln );\
    }\
}\
\
LN_FUNC_SIGNATURE(LN(type)*, CopyList, type, LN(type)* list)\
{\
    if ( list == NULL )\
        return NULL;\
    \
    return Create_LN_##type(list->data, CopyList_LN_##type(list->next));\
}\
LN_FUNC_SIGNATURE(void, DestroyList, type, LN(type)* list)\
{\
    if ( list == NULL )\
        return;\
\
    LN(type)* ln = list;\
    while ( ln != NULL )\
    {\
        LN(type)* next = ln->next;\
        Destroy_LN_##type( ln );\
        ln = next;\
    }\
}\

/* Makes writing out Set types easier */
#define SET(a) CONCAT(Set, a)
#define SET_FUNC_NAME(name, a) EVAL(name, SET(a))
#define SET_FUNC_SIGNATURE(ret, name, a, ...) function_signature(ret, SET_FUNC_NAME(name, a), __VA_ARGS__)

#define SET_FUNC(name, t, ...) EVAL(name, EVAL(Set, t))(__VA_ARGS__)

#define DEFINE_SET(type)\
typedef struct SET(type) SET(type);\
struct SET(type)\
{\
    int used;\
    int bins;\
    float loadThreshold;\
    int modCount;\
    LN(type)** data;\
};\
\
SET_FUNC_SIGNATURE(void, EnsureLoadThreshold, type, SET(type)* s, int newUsed);\
SET_FUNC_SIGNATURE(SET(type)*, Create, type, int bins, float loadThreshold)\
{\
    SET(type)* s = malloc( sizeof( struct SET(type) ) );\
    s->bins = bins >= 1 ? bins : 1;\
    s->loadThreshold = loadThreshold >= 0.0f ? loadThreshold : 0.75f;\
    s->used = 0;\
    s->modCount = 0;\
    s->data = malloc( s->bins * sizeof( LN(type)* ) );\
    memset( s->data, 0, s->bins * sizeof( LN(type)* ) );\
    return s;\
}\
\
SET_FUNC_SIGNATURE(SET(type)*, Copy, type, SET(type)* set)\
{\
    SET(type)* s = malloc( sizeof( struct SET(type)) );\
    s->bins = set->bins;\
    s->loadThreshold = set->loadThreshold; \
    s->used = set->used;\
    s->modCount = 0;\
    s->data = malloc( s->bins * sizeof( LN(type)* ) );\
    int i = 0;\
    for (; i < s->bins; ++i)\
    {\
        s->data[i] = LN_FUNC(CopyList, type, set->data[i]);\
    }\
    return s;\
}\
SET_FUNC_SIGNATURE(SET(type)*, CreateDefault, type, void)\
{\
    return SET_FUNC(Create, type, 1, 0.75f);\
}\
\
SET_FUNC_SIGNATURE(bool, Equals, type, SET(type)* lhs, SET(type*) rhs)\
{\
    return TRUE;\
}\
SET_FUNC_SIGNATURE(void, Swap, type, SET(type)** lhs, SET(type)** rhs)\
{\
    SET(type)* t = *lhs;\
    *lhs = *rhs;\
    *rhs = t;\
}\
SET_FUNC_SIGNATURE(void, Destroy, type, SET(type)* s)\
{\
    if ( s && s->data )\
    {\
        int i;\
        for ( i = 0; i < s->bins; ++i )\
        {\
            LN_FUNC(DestroyList, type, s->data[i]);\
            s->data[ i ] = NULL;\
        }\
        free(s->data);\
    }\
    free( s );\
}\
\
SET_FUNC_SIGNATURE(int, Insert, type, SET(type)* s, type data)\
{\
    if ( s->data )\
    {\
        SET_FUNC(EnsureLoadThreshold, type, s, s->used + 1 );\
        unsigned long hash = hash_##type( data );\
        int index = abs( ( int ) hash ) % s->bins;\
        if ( s->data[ index ] == NULL )\
        {\
            s->data[ index ] = LN_FUNC(Create, type, data, NULL);\
            ++s->modCount;\
            ++s->used;\
            return 1;\
        }\
        else\
        {\
            LN(type)* ln = NULL;\
            for ( ln = s->data[ index ]; ln != NULL; ln = ln->next )\
            {\
                if ( equals_##type( ln->data, data ) == TRUE )\
                {\
                    return 0;\
                }\
                else if ( ln->next == NULL )\
                {\
                    break;\
                }\
            }\
            ++s->modCount;\
            ++s->used;\
            ln->next = LN_FUNC(Create, type, data, NULL );\
            return 1;\
        }\
    }\
\
    return 0;\
}\
\
SET_FUNC_SIGNATURE(int, Remove, type, SET(type)* s, type data)\
{\
    if ( s->data )\
    {\
        unsigned long hash = hash_##type( data );\
        int index = abs( ( int ) hash ) % s->bins;\
        if ( s->data[ index ] != NULL )\
        {\
            LN(type)** head = &s->data[ index ];\
            LN(type)* prev = *head;\
            LN(type)* ln = *head;\
            for ( ; ln != NULL; prev = ln, ln = ln->next )\
            {\
                if ( equals_##type( ln->data, data ) == TRUE )\
                {\
                    LN(type)* n = ln->next;\
                    if ( ln == *head )\
                    {\
                        *head = n;\
                    }\
                    else\
                    {\
                        prev->next = n;\
                    }\
                    LN_FUNC(Destroy, type, ln );\
                    --s->used;\
                    ++s->modCount;\
                    return 1;\
                }\
            }\
        }\
    }\
\
    return 0;\
}\
\
SET_FUNC_SIGNATURE(bool, Contains, type, SET(type)* s, type data)\
{\
    if ( s->data )\
    {\
        unsigned long hash = hash_##type( data );\
        int index = abs( ( int ) hash ) % s->bins;\
        if ( s->data[ index ] != NULL )\
        {\
            LN(type)* ln = NULL;\
            for ( ln = s->data[ index ]; ln != NULL; ln = ln->next )\
            {\
                if ( equals_##type( ln->data, data ) == TRUE )\
                {\
                    return TRUE;\
                }\
            }\
        }\
    }\
    \
    return FALSE;\
}\
\
SET_FUNC_SIGNATURE(bool, IsEmpty, type, SET(type)* set)\
{\
    if ( set->used == 0 )\
    {\
        return TRUE;\
    }\
    return FALSE;\
}\
\
SET_FUNC_SIGNATURE(void, EnsureLoadThreshold, type, SET(type)* s, int newUsed)\
{\
    float newLoadFactor = ( ( float ) s->used ) / s->bins;\
    if ( newLoadFactor >= s->loadThreshold )\
    {\
        LN(type)** oldData = s->data;\
        int oldBins = s->bins;\
        \
        s->bins = 2 * s->bins;\
        s->used = 0;\
        s->modCount = 0;\
        \
        s->data = malloc( s->bins * sizeof( LN(type)* ) );\
        memset( s->data, 0, s->bins * sizeof( LN(type)* ) );\
        \
        int i;\
        for ( i = 0; i < oldBins; ++i )\
        {\
            LN(type)* head = oldData[i];\
            LN(type)* ln = head;\
            for ( ; ln != NULL; ln = ln->next )\
            {\
                SET_FUNC(Insert, type, s, ln->data );\
            }\
            LN_FUNC(DestroyList, type, head ); \
        }\
        \
        free(oldData);\
    }\
}\

/* Makes writing out Map types easier */
#define MAP(a, b) EVAL(Map, CONCAT(a, b))
#define MAP_FUNC_NAME(name, a, b) EVAL(name, MAP(a, b))
#define MAP_FUNC_SIGNATURE(ret, name, a, b, ...) function_signature(ret, MAP_FUNC_NAME(name, a, b), __VA_ARGS__)

#define MAP_FUNC(name, k, v,...) EVAL(name, MAP(k, v))(__VA_ARGS__)

#define DEFINE_MAP(k, v)\
typedef struct MAP(k, v) MAP(k, v);\
struct MAP(k, v)\
{\
    int used;\
    int bins;\
    float loadThreshold;\
    int modCount;\
    LN(PAIR(k,v))** data;\
};\
\
MAP_FUNC_SIGNATURE(void, EnsureLoadThreshold, k, v, MAP(k,v) * m, int newUsed );\
MAP_FUNC_SIGNATURE(MAP(k, v)*, Create, k, v, int bins, float loadThreshold)\
{\
    MAP(k, v)* m = malloc(sizeof( struct MAP(k, v) ));\
    m->bins = bins >= 1 ? bins : 1;\
    m->loadThreshold = loadThreshold >= 0.0f ? loadThreshold : 0.75f;\
    m->used = 0;\
    m->modCount = 0;\
    m->data = malloc(m->bins * sizeof( LN(PAIR(k,v))*));\
    memset(m->data, 0, m->bins * sizeof(LN(PAIR(k,v)) *));\
    return m;\
}\
MAP_FUNC_SIGNATURE(void, Destroy, k,v, MAP(k, v)* m)\
{\
    if ( m && m->data )\
    {\
        int i;\
        for ( i = 0; i < m->bins; ++i )\
        {\
            LN_FUNC(DestroyList, PAIR(k,v),m->data[i]);\
            m->data[ i ] = NULL;\
        }\
        free(m->data);\
    }\
    free( m );\
}\
MAP_FUNC_SIGNATURE(bool, HasKey, k, v, MAP(k, v)* m, k key)\
{\
    if ( m->data )\
    {\
        unsigned long hash = hash_##k( key );\
        int index = abs( ( int ) hash ) % m->bins;\
        \
        if ( m->data[ index ] != NULL )\
        {\
            LN(PAIR(k,v))* ln = NULL;\
            for ( ln = m->data[ index ]; ln != NULL; ln = ln->next )\
            {\
                if ( equals_##k( ln->data.first, key ) == TRUE )\
                {\
                    return TRUE;\
                }\
            }\
        }\
    }\
\
    return FALSE;\
}\
MAP_FUNC_SIGNATURE(v*, AddKey, k, v, MAP(k, v)* m, k key )\
{\
    if ( m->data )\
    {\
        MAP_FUNC(EnsureLoadThreshold, k, v,m, m->used + 1);\
        unsigned long hash = hash_##k( key );\
        int index = abs( ( int ) hash ) % m->bins;\
        if ( m->data[ index ] == NULL )\
        {\
            v defaultV = alloc_##v();\
            PAIR(k,v) pair = PAIR_FUNC(Create, k , v, key, defaultV );\
            free_##v(defaultV);\
            m->data[ index ] = LN_FUNC(Create, PAIR(k, v), pair , NULL );\
            PAIR_FUNC(Destroy, k, v,pair);\
            ++m->modCount;\
            ++m->used;\
            return &m->data[ index ]->data.second;\
        }\
        else\
        {\
            LN(PAIR(k,v))* ln = NULL;\
            for ( ln = m->data[ index ];; ln = ln->next )\
            {\
                if ( equals_##k( ln->data.first, key ) == TRUE )\
                {\
                    \
                    return &ln->data.second;\
                }\
                else if ( ln->next == NULL )\
                {\
                    break;\
                }\
            }\
            ++m->modCount;\
            ++m->used;\
            v defaultV = alloc_##v();\
            PAIR(k,v) pair = PAIR_FUNC(Create, k , v, key, defaultV );\
            free_##v(defaultV);\
            ln->next = LN_FUNC(Create, PAIR(k, v), pair, NULL );\
            PAIR_FUNC(Destroy, k,v,pair);\
            return &ln->next->data.second;\
        }\
    }\
\
    return NULL;\
}\
\
MAP_FUNC_SIGNATURE( int, Remove, k, v, MAP(k, v)* m, k key );\
MAP_FUNC_SIGNATURE( int, RemovePair, k, v, MAP(k, v)* m, PAIR(k,v) p )\
{\
    return MAP_FUNC(Remove, k, v,m, p.first);\
}\
MAP_FUNC_SIGNATURE( int, Remove, k, v, MAP(k, v)* m, k key )\
{\
    if ( m->data )\
    {\
        unsigned long hash = hash_##k( key );\
        int index = abs( ( int ) hash ) % m->bins;\
        if ( m->data[ index ] != NULL )\
        {\
            LN(PAIR(k,v))** head = &m->data[ index ];\
            LN(PAIR(k,v))* prev = *head;\
            LN(PAIR(k,v))* ln = *head;\
            for ( ; ln != NULL; prev = ln, ln = ln->next )\
            {\
                if ( equals_##k( ln->data.first, key ) == TRUE )\
                {\
                    \
                    \
                    LN(PAIR(k,v))* n = ln->next;\
                    if ( ln == *head )\
                    {\
                        *head = n;\
                    }\
                    else\
                    {\
                        prev->next = n;\
                    }\
                    LN_FUNC(Destroy, PAIR(k, v), ln );\
                    ++m->modCount;\
                    --m->used;\
                    return 1;\
                }\
            }\
        }\
    }\
    return 0;\
}\
\
MAP_FUNC_SIGNATURE(v*, GetValue, k, v, MAP(k, v)* m, k key )\
{\
    return MAP_FUNC(AddKey, k, v, m, key );\
}\
MAP_FUNC_SIGNATURE(void, EnsureLoadThreshold, k, v, MAP(k,v) * m, int newUsed )\
{\
    float newLoadFactor = ( ( float ) m->used ) / m->bins;\
    if ( newLoadFactor >= m->loadThreshold )\
    {\
        LN(PAIR(k,v))** oldData = m->data;\
        int oldBins = m->bins;\
        \
        m->bins = 2 * m->bins;\
        m->used = 0;\
        m->modCount = 0;\
        \
        m->data = malloc( m->bins * sizeof( LN(PAIR(k,v))* ) );\
        memset( m->data, 0, m->bins * sizeof( LN(PAIR(k,v))* ) );\
        \
        int i;\
        for ( i = 0; i < oldBins; ++i )\
        {\
            LN(PAIR(k,v))* head = oldData[i];\
            LN(PAIR(k,v))* ln = head;\
            for ( ; ln != NULL; ln = ln->next )\
            {\
                v* value = MAP_FUNC(AddKey, k,v , m, ln->data.first );\
                swap_##v(value, &ln->data.second);\
            }\
            LN_FUNC(DestroyList, PAIR(k,v), head ); \
        }\
        \
        free(oldData);\
    }\
}\

#define USE_DEFAULT_ITER_REMOVE(type, container)\
function_signature(int, remove_##container, container* refContainer, type value){}

#define USE_AS_ITER_REMOVE(type, container, func)\
define_func_alias(int, remove_##container, func, container*, type)

/* Makes writing out Iter types easier */
#define ITER(a) CONCAT(Iter, a)
#define ITER_FUNC_NAME(name, a) EVAL(name, ITER(a))
#define ITER_FUNC_SIGNATURE(ret, name, a, ...) function_signature(ret, ITER_FUNC_NAME(name, a), __VA_ARGS__)

#define ITER_FUNC(name, t, ...) EVAL(name, EVAL(Iter, t))(__VA_ARGS__)

#define DEFINE_ITER(type, container)\
typedef struct ITER(container) ITER(container);\
struct ITER(container)\
{\
    container* refContainer;\
    LN(type)* ln;\
    int bucketIndex;\
    int expectedModCount;\
};\
\
ITER_FUNC_SIGNATURE(int, Incr, container, ITER(container)* iter);\
ITER_FUNC_SIGNATURE(ITER(container)*, Create, container, container* refContainer)\
{\
    ITER(container)* iter = malloc( sizeof( struct ITER(container) ) );\
    if ( iter )\
    {\
        iter->bucketIndex = 0;\
        iter->expectedModCount = refContainer->modCount;\
        iter->refContainer = refContainer;\
        iter->ln = NULL;\
        ITER_FUNC(Incr, container, iter ); \
    }\
    return iter;\
}\
\
ITER_FUNC_SIGNATURE(void, Destroy, container, ITER(container)* iter)\
{\
    free( iter );\
}\
\
ITER_FUNC_SIGNATURE(int, Incr, container, ITER(container)* iter)\
{\
    if ( iter )\
    {\
        if (iter->expectedModCount != iter->refContainer->modCount)\
        {\
            return -1;\
        }\
        if ( iter->ln )\
        {\
            \
            if ( iter->ln->next != NULL )\
            {\
                iter->ln = iter->ln->next;\
            }\
            else\
            {\
                \
                ++iter->bucketIndex;\
                while ( iter->bucketIndex < iter->refContainer->bins && iter->refContainer->data[ iter->bucketIndex ] == NULL )\
                {\
                    ++iter->bucketIndex;\
                }\
                 \
                if ( iter->bucketIndex < iter->refContainer->bins )\
                {\
                    iter->ln = iter->refContainer->data[ iter->bucketIndex ];\
                }\
                else\
                {\
                    iter->ln = NULL;\
                }\
            }\
        }\
        else \
        {\
            while ( iter->bucketIndex < iter->refContainer->bins && iter->refContainer->data[ iter->bucketIndex ] == NULL )\
            {\
                ++iter->bucketIndex;\
            }\
                \
            if ( iter->bucketIndex < iter->refContainer->bins )\
            {\
                iter->ln = iter->refContainer->data[ iter->bucketIndex ];\
            }\
            else\
            {\
                iter->ln = NULL;\
            }\
                \
        }\
    return 0;\
    }\
    return 1;\
}\
\
ITER_FUNC_SIGNATURE(type*, Deref, container, ITER(container)* iter)\
{\
    if ( iter->bucketIndex < iter->refContainer->bins && iter->refContainer && iter->ln && iter->refContainer->modCount == iter->expectedModCount)\
    {\
        return &iter->ln->data;\
    }\
    return NULL;\
}\
\
ITER_FUNC_SIGNATURE(int, RemoveCurrent, container, ITER(container)* iter)\
{\
    if ( iter->bucketIndex < iter->refContainer->bins && iter->refContainer && iter->ln && iter->refContainer->modCount == iter->expectedModCount )\
    {\
        int ret = remove_##container( iter->refContainer, iter->ln->data );\
        iter->expectedModCount = iter->refContainer->modCount;\
        iter->ln = NULL;\
        return ret;\
    }\
    return 0;\
}\
\
ITER_FUNC_SIGNATURE(bool, IsEnd, container, ITER(container)* iter)\
{\
    if ( iter->bucketIndex >= iter->refContainer->bins || iter->ln == NULL )\
    {\
        return TRUE;\
    }\
        \
    return FALSE;\
}\

/* Makes writing out Array types easier */
#define ARRAY(a) CONCAT(Array, a)
#define ARRAY_FUNC_NAME(name, a) EVAL(name, ARRAY(a))
#define ARRAY_FUNC_SIGNATURE(ret, name, a, ...) function_signature(ret, ARRAY_FUNC_NAME(name, a), __VA_ARGS__)

#define ARRAY_FUNC(name, t, ...) EVAL(name, EVAL(Array, t))(__VA_ARGS__)

/* Array type that acts like a mix between vector and a dynamic array - its elements are guaranteed
 * to be initialized.
 */
#define DEFINE_ARRAY(type)\
typedef struct ARRAY(type) ARRAY(type);\
struct ARRAY(type)\
{\
    int size;\
    type* data;\
};\
ARRAY_FUNC_SIGNATURE(void, EnsureSize, type, ARRAY(type)* v, int newSize);\
ARRAY_FUNC_SIGNATURE(ARRAY(type)*, Create, type, int size)\
{\
    ARRAY(type)* v = malloc(sizeof(struct ARRAY(type)));\
    v->size = size >= 0 ? size : 1;\
    v->data = malloc(v->size * sizeof(type));\
    memset(v->data, 0, v->size * sizeof(type));\
    return v;\
}\
ARRAY_FUNC_SIGNATURE(ARRAY(type)*, Copy, type, ARRAY(type)* v)\
{\
    ARRAY(type)* cpy = malloc(sizeof(struct ARRAY(type)));\
    cpy->size = v->size;\
    cpy->data = malloc(cpy->size * sizeof(type));\
    int i = 0;\
    for ( ; i < cpy->size; ++i )\
    {\
        cpy->data[i] = copy_##type(v->data[i]);\
    }\
    \
    return cpy;\
}\
ARRAY_FUNC_SIGNATURE(void, Destroy, type, ARRAY(type)* v)\
{\
    int i = 0;\
    for ( i = 0; i < v->size; ++i )\
    {\
        free_##type( v->data[ i ] );\
    }\
    free( v->data );\
    free(v);\
}\
ARRAY_FUNC_SIGNATURE(void, Resize, type, ARRAY(type)* v, int newSize)\
{\
    if (newSize == v->size) return;\
    else\
    {\
        type* oldData = v->data;\
        int oldCapacity = v->size;\
        v->size = newSize;\
        v->data = malloc(v->size * sizeof(type));\
        int i = 0;\
        for ( ; i < oldCapacity; ++i )\
        {\
            v->data[i] = copy_##type(oldData[i]);\
            free_##type(oldData[i]);\
        }\
        free(oldData);\
        \
    }\
}\
ARRAY_FUNC_SIGNATURE(void, PushBack, type, ARRAY(type)* v, type data)\
{\
    if ( v && v->data)\
    {\
        ARRAY_FUNC(EnsureSize, type, v, v->size + 1);\
        ++v->size;\
        type* oldData = &v->data[v->size - 1];\
        v->data[v->size - 1] = copy_##type(data);\
        free_##type(*oldData);\
    }\
}\
ARRAY_FUNC_SIGNATURE(type*, At, type, ARRAY(type)* v, int index)\
{\
    if ( index < 0 || index >= v->size)\
        return NULL;\
    \
    return &v->data[ index ];\
}\
ARRAY_FUNC_SIGNATURE(void, EnsureSize, type, ARRAY(type)* v, int newSize)\
{\
    if ( newSize >= v->size )\
    {\
        ARRAY_FUNC(Resize, type, v, newSize);\
    }\
}\

typedef char* pchar;
unsigned long hash_djb2(char* str);
unsigned long HashString(pchar str);
pchar DefaultString(void);
pchar CopyString(pchar str);
void FreeString(pchar str);
bool EqualsString(pchar lhs, pchar rhs);

USE_DEFAULT_ALLOC(int)
USE_DEFAULT_COPY(int)
USE_DEFAULT_EQUALS(int)
USE_DEFAULT_FREE(int)
USE_DEFAULT_SWAP(int)

USE_AS_ALLOC(pchar, DefaultString)
USE_AS_COPY(pchar, CopyString)
USE_AS_EQUALS(pchar, EqualsString)
USE_AS_FREE(pchar, FreeString)
USE_AS_HASH(pchar, HashString)
USE_DEFAULT_SWAP(pchar)

DEFINE_LN(pchar)
DEFINE_SET(pchar)
USE_AS_ITER_REMOVE(pchar, Set_pchar, Remove_Set_pchar)
DEFINE_ITER(pchar, Set_pchar)

typedef Set_pchar* PtrSet_pchar;
USE_AS_ALLOC(PtrSet_pchar, CreateDefault_Set_pchar)
USE_AS_COPY(PtrSet_pchar, Copy_Set_pchar)
USE_AS_FREE(PtrSet_pchar, Destroy_Set_pchar)
USE_AS_SWAP(PtrSet_pchar, Swap_Set_pchar)
USE_DEFAULT_EQUALS(PtrSet_pchar)

DEFINE_PAIR(pchar, PtrSet_pchar)
USE_AS_COPY(Pair_pchar_PtrSet_pchar, Copy_Pair_pchar_PtrSet_pchar)
USE_AS_FREE(Pair_pchar_PtrSet_pchar, Destroy_Pair_pchar_PtrSet_pchar)
DEFINE_LN(Pair_pchar_PtrSet_pchar)
DEFINE_MAP(pchar, PtrSet_pchar)

USE_AS_ITER_REMOVE(Pair_pchar_PtrSet_pchar, Map_pchar_PtrSet_pchar, RemovePair_Map_pchar_PtrSet_pchar)
DEFINE_ITER(Pair_pchar_PtrSet_pchar, Map_pchar_PtrSet_pchar)

typedef struct edge* Edge;
typedef struct nodeInfo* NodeInfo;

Edge CreateEdge(void);
bool EqualsEdge(Edge lhs, Edge rhs);
Edge CopyEdge(Edge v);
void FreeEdge(Edge e);
unsigned long HashEdge(Edge e);

NodeInfo CreateNodeInfo(void);
NodeInfo CopyNodeInfo(NodeInfo nodeInfo);
void FreeNodeInfo(NodeInfo nodeInfo);
bool EqualsNodeInfo(NodeInfo lhs, NodeInfo rhs);
void SwapNodeInfo(NodeInfo* lhs, NodeInfo* rhs);

USE_AS_ALLOC(Edge, CreateEdge)
USE_AS_COPY(Edge, CopyEdge)
USE_AS_EQUALS(Edge, EqualsEdge)
USE_AS_FREE(Edge, FreeEdge)
USE_AS_HASH(Edge, HashEdge)

DEFINE_LN(Edge)
DEFINE_SET(Edge)
USE_AS_ITER_REMOVE(Edge, Set_Edge, Remove_Set_Edge)
DEFINE_ITER(Edge, Set_Edge)

DEFINE_PAIR(Edge, Edge)
USE_AS_ALLOC(Pair_Edge_Edge, Alloc_Pair_Edge_Edge)
USE_AS_COPY(Pair_Edge_Edge, Copy_Pair_Edge_Edge)
USE_AS_EQUALS(Pair_Edge_Edge, Equals_Pair_Edge_Edge)
USE_AS_FREE(Pair_Edge_Edge, Destroy_Pair_Edge_Edge)

USE_AS_ALLOC(NodeInfo, CreateNodeInfo)
USE_AS_COPY(NodeInfo, CopyNodeInfo)
USE_AS_FREE(NodeInfo, FreeNodeInfo)
USE_AS_EQUALS(NodeInfo, EqualsNodeInfo)
USE_AS_SWAP(NodeInfo, SwapNodeInfo)
DEFINE_PAIR(pchar, NodeInfo)
USE_AS_ALLOC(Pair_pchar_NodeInfo, Alloc_Pair_pchar_NodeInfo)
USE_AS_COPY(Pair_pchar_NodeInfo, Copy_Pair_pchar_NodeInfo)
USE_AS_EQUALS(Pair_pchar_NodeInfo, Equals_Pair_pchar_NodeInfo)
USE_AS_FREE(Pair_pchar_NodeInfo, Destroy_Pair_pchar_NodeInfo)
DEFINE_LN(Pair_pchar_NodeInfo)
DEFINE_MAP(pchar, NodeInfo)

USE_AS_ITER_REMOVE(Pair_pchar_NodeInfo, Map_pchar_NodeInfo, RemovePair_Map_pchar_NodeInfo)
DEFINE_ITER(Pair_pchar_NodeInfo, Map_pchar_NodeInfo)

DEFINE_PAIR(pchar, pchar)
USE_AS_ALLOC(Pair_pchar_pchar, Alloc_Pair_pchar_pchar)
USE_AS_COPY(Pair_pchar_pchar, Copy_Pair_pchar_pchar)
USE_AS_EQUALS(Pair_pchar_pchar, Equals_Pair_pchar_pchar)
USE_AS_FREE(Pair_pchar_pchar, Destroy_Pair_pchar_pchar)
DEFINE_LN(Pair_pchar_pchar)
DEFINE_MAP(pchar, pchar)

USE_AS_ITER_REMOVE(Pair_pchar_pchar, Map_pchar_pchar, RemovePair_Map_pchar_pchar)
DEFINE_ITER(Pair_pchar_pchar, Map_pchar_pchar)

DEFINE_PAIR(pchar, int)
USE_AS_ALLOC(Pair_pchar_int, Alloc_Pair_pchar_int)
USE_AS_COPY(Pair_pchar_int, Copy_Pair_pchar_int)
USE_AS_EQUALS(Pair_pchar_int, Equals_Pair_pchar_int)
USE_AS_FREE(Pair_pchar_int, Destroy_Pair_pchar_int)
DEFINE_LN(Pair_pchar_int)
DEFINE_MAP(pchar, int)

USE_AS_ITER_REMOVE(Pair_pchar_int, Map_pchar_int, RemovePair_Map_pchar_int)
DEFINE_ITER(Pair_pchar_int, Map_pchar_int)

DEFINE_ARRAY(PtrSet_pchar)
typedef Array_PtrSet_pchar* PtrArray_PtrSet_pchar;
USE_AS_FREE(PtrArray_PtrSet_pchar, Destroy_Array_PtrSet_pchar)
USE_AS_COPY(PtrArray_PtrSet_pchar, Copy_Array_PtrSet_pchar)
DEFINE_ARRAY(PtrArray_PtrSet_pchar)


typedef struct Graph Graph;
struct Graph
{
    MAP(pchar, NodeInfo)* nodeMap;
    SET(Edge)* edgeSet;
};

Graph* Create_Graph();
void Destroy_Graph(Graph* g);
void AddNode_Graph(Graph* g, pchar node);
void AddEdge_Graph(Graph* g, pchar from, pchar to);
void RemoveNode_Graph(Graph* g, pchar node);
void RemoveEdge_Graph(Graph* g, pchar from, pchar to);
bool HasNode_Graph(Graph* g, pchar node);
bool HasEdge_Graph(Graph* g, pchar from, pchar to);
int InDegree_Graph(Graph* g, pchar node);
int OutDegree_Graph(Graph* g, pchar node);
void Print_Graph(Graph* g);

typedef struct EquivalenceClass EquivalenceClass;
struct EquivalenceClass
{
    MAP(pchar, pchar)* parentMap;
    MAP(pchar, int)* rootSizeMap;
};
EquivalenceClass* CreateEquivalenceClass();
void DestroyEquivalenceClass(EquivalenceClass* ec);
void AddElement(EquivalenceClass* ec, pchar c);
void MergeClasses(EquivalenceClass* ec, pchar c1, pchar c2);
bool InSameClass(EquivalenceClass* ec, pchar c1, pchar c2);
SET(pchar)* GetClassOf(EquivalenceClass* ec, pchar c);
pchar CompressToRoot(EquivalenceClass* ec, pchar c);

enum Quarters { FALL, WINTER, SPRING, NUM_QTRS };
typedef Array_PtrArray_PtrSet_pchar CoursePlan;

CoursePlan* InitCoursePlan(int initialNumYears);
void FreeCoursePlan(CoursePlan* plan);
void AddCourse(CoursePlan* plan, int year, int quarter, pchar course);
PtrSet_pchar GetCoursesAt(CoursePlan* plan, int year, int quarter);
void PrintCoursePlan(CoursePlan* plan);
void ResizeCoursePlan(CoursePlan* plan, int newSize);

SET(pchar)* GetAllNecessaryCourses(Graph* g, SET(pchar)* reqSet);
void CreateCoursePlan(FILE* offeringFile, FILE* prereqFile, FILE* reqFile, CoursePlan* outPlan);

int SkipWhitespace(FILE* file);
void ReadRequiredCourses(FILE* reqFile, Graph* g, SET(pchar)* outreqSet);
void ReadPrerequisites(FILE* prereqFile, Graph* g);
void ReadOfferedCourses(FILE* offeringFile, SET(pchar)* necessaryCourses, SET(pchar)* reqCourses, SET(pchar)** outOfferedCourses);

USE_DEFAULT_HASH(Pair_pchar_int)
DEFINE_SET(Pair_pchar_int)
int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        return -1;
    }

    FILE* offeringFile = fopen(argv[OFFERING_FILE], "r");
    FILE* prereqFile = fopen(argv[PREREQ_FILE], "r");
    FILE* reqFile = fopen(argv[REQ_FILE], "r");

    if (!offeringFile || !prereqFile || !reqFile)
    {
        return -1;
    }

    CoursePlan* plan = InitCoursePlan(10);

    CreateCoursePlan(offeringFile, prereqFile, reqFile, plan);
    PrintCoursePlan(plan);

    FreeCoursePlan(plan);
}

struct edge
{
    pchar from;
    pchar to;
};

struct nodeInfo
{
    pchar theNode;
    SET(pchar)* outNodes;
    SET(pchar)* inNodes;
    SET(Edge)* outEdges;
    SET(Edge)* inEdges;
};

unsigned long hash_djb2(char* str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

unsigned long HashString(pchar str)
{
    return hash_djb2(str);
}

pchar DefaultString(void)
{
    pchar str = malloc(sizeof(char) * MAX_COURSENAME_SIZE);
    memset(str, 0, sizeof(char) * MAX_COURSENAME_SIZE);
    return (pchar)str;
}

pchar CopyString(pchar str)
{
    pchar cpy = malloc(sizeof(char) * MAX_COURSENAME_SIZE);
    memset(cpy, 0, sizeof(char) * MAX_COURSENAME_SIZE);
    strcpy(cpy, str);
    return cpy;
}

void FreeString(pchar str)
{
    free(str);
}

bool EqualsString(pchar lhs, pchar rhs)
{
    return strcmp(lhs, rhs) == 0 ? TRUE : FALSE;
}

Edge CreateEdge(void)
{
    Edge e = malloc(sizeof(struct edge));
    e->from = DefaultString();
    e->to = DefaultString();
    return e;
}

bool EqualsEdge(Edge lhs, Edge rhs)
{
    if (EqualsString(lhs->from, rhs->from) == TRUE && EqualsString(lhs->to, rhs->to) == TRUE)
        return TRUE;

    return FALSE;
}

Edge CopyEdge(Edge v)
{
    Edge e = malloc(sizeof(struct edge));
    e->from = CopyString(v->from);
    e->to = CopyString(v->to);
    return e;
}

void FreeEdge(Edge e)
{
    if (e)
    {
        FreeString(e->from);
        FreeString(e->to);
        free(e);
    }
}

unsigned long HashEdge(Edge e)
{
    return HashString(e->from) + HashString(e->to);
}

NodeInfo CreateNodeInfo(void)
{
    NodeInfo info = malloc(sizeof(struct nodeInfo));
    info->inEdges = SET_FUNC(Create, Edge, 30, 0.75f);
    info->outEdges = SET_FUNC(Create, Edge, 30, 0.75f);
    info->inNodes = SET_FUNC(Create, pchar, 30, 0.75f);
    info->outNodes = SET_FUNC(Create, pchar, 30, 0.75f);
    info->theNode = DefaultString();
    return info;
}

NodeInfo CopyNodeInfo(NodeInfo nodeInfo)
{
    NodeInfo info = malloc(sizeof(struct nodeInfo));
    info->inEdges = SET_FUNC(Copy, Edge, nodeInfo->inEdges);
    info->outEdges = SET_FUNC(Copy, Edge, nodeInfo->outEdges);
    info->inNodes = SET_FUNC(Copy, pchar, nodeInfo->inNodes);
    info->outNodes = SET_FUNC(Copy, pchar, nodeInfo->outNodes);
    info->theNode = CopyString(nodeInfo->theNode);
    return info;
}

void FreeNodeInfo(NodeInfo nodeInfo)
{
    FreeString(nodeInfo->theNode);
    SET_FUNC(Destroy, Edge, nodeInfo->inEdges);
    SET_FUNC(Destroy, Edge, nodeInfo->outEdges);
    SET_FUNC(Destroy, pchar, nodeInfo->inNodes);
    SET_FUNC(Destroy, pchar, nodeInfo->outNodes);
    free(nodeInfo);
}

bool EqualsNodeInfo(NodeInfo lhs, NodeInfo rhs)
{
    return EqualsString(lhs->theNode, rhs->theNode);
}

void SwapNodeInfo(NodeInfo* lhs, NodeInfo* rhs)
{
    NodeInfo t = *lhs;
    *lhs = *rhs;
    *rhs = t;
}

Graph* Create_Graph()
{
    Graph* g = malloc(sizeof(Graph));
    g->nodeMap = MAP_FUNC(Create, pchar, NodeInfo, 30, 0.75f);
    g->edgeSet = SET_FUNC(Create, Edge, 30, 0.75f);
    return g;
}

void Destroy_Graph(Graph* g)
{
    MAP_FUNC(Destroy, pchar, NodeInfo, g->nodeMap);
    SET_FUNC(Destroy, Edge, g->edgeSet);
    free(g);
}

void AddNode_Graph(Graph* g, pchar node)
{
    if (HasNode_Graph(g, node) == TRUE)
        return;

    NodeInfo* nodeInfo = MAP_FUNC(AddKey, pchar, NodeInfo, g->nodeMap, node);
    strcpy((*nodeInfo)->theNode, node);
}

void AddEdge_Graph(Graph* g, pchar from, pchar to)
{
    if (HasEdge_Graph(g, from, to) == TRUE)
        return;

    Edge e = CreateEdge();
    strcpy(e->from, from);
    strcpy(e->to, to);

    /* Add to graph's edge set and to the relevant sets for the node info of origin and dest*/
    SET_FUNC(Insert, Edge, g->edgeSet, e);
    NodeInfo nodeInfoFrom = *MAP_FUNC(GetValue, pchar, NodeInfo, g->nodeMap, e->from);
    SET_FUNC(Insert, Edge, nodeInfoFrom->outEdges, e);
    SET_FUNC(Insert, pchar, nodeInfoFrom->outNodes, e->to);
    NodeInfo nodeInfoTo = *MAP_FUNC(GetValue, pchar, NodeInfo, g->nodeMap, e->to);
    SET_FUNC(Insert, Edge, nodeInfoTo->inEdges, e);
    SET_FUNC(Insert, pchar, nodeInfoTo->inNodes, e->to);
    FreeEdge(e);
}

void RemoveNode_Graph(Graph* g, pchar node)
{
    if (HasNode_Graph(g, node) != TRUE)
        return;

    /* Remove from NodeInfo map */
    MAP_FUNC(Remove, pchar, NodeInfo, g->nodeMap, node);

    /* Remove from data in each NodeInfo Entry*/
    ITER(MAP(pchar, NodeInfo))* iter = ITER_FUNC(Create, MAP(pchar, NodeInfo), g->nodeMap);
    while (ITER_FUNC(IsEnd, MAP(pchar, NodeInfo), iter) != TRUE)
    {
        PAIR(pchar, NodeInfo)* pair = ITER_FUNC(Deref, MAP(pchar, NodeInfo), iter);

        NodeInfo info = pair->second;
        SET_FUNC(Remove, pchar, info->outNodes, node);
        SET_FUNC(Remove, pchar, info->inNodes, node);

        /* Remove from out Edge set */
        ITER(SET(Edge))* iterOutEdges = ITER_FUNC(Create, SET(Edge), info->outEdges);
        while (ITER_FUNC(IsEnd, SET(Edge), iterOutEdges) != TRUE)
        {
            Edge graphEdge = *ITER_FUNC(Deref, SET(Edge), iterOutEdges);
            if (EqualsString(graphEdge->to, node) == TRUE)
            {
                SET_FUNC(Remove, Edge, g->edgeSet, graphEdge);
                ITER_FUNC(RemoveCurrent, SET(Edge), iterOutEdges);
            }
            if (ITER_FUNC(Incr, SET(Edge), iterOutEdges) != 0) break;
        }
        ITER_FUNC(Destroy, SET(Edge), iterOutEdges);

        /* Remove from in Edge set */
        ITER(SET(Edge))* iterInEdges = ITER_FUNC(Create, SET(Edge), info->inEdges);
        while (ITER_FUNC(IsEnd, SET(Edge), iterInEdges) != TRUE)
        {
            Edge graphEdge = *ITER_FUNC(Deref, SET(Edge), iterInEdges);
            if (EqualsString(graphEdge->from, node) == TRUE)
            {
                SET_FUNC(Remove, Edge, g->edgeSet, graphEdge);
                ITER_FUNC(RemoveCurrent, SET(Edge), iterInEdges);
            }
            if (ITER_FUNC(Incr, SET(Edge), iterInEdges) != 0) break;
        }
        ITER_FUNC(Destroy, SET(Edge), iterInEdges);

        if (ITER_FUNC(Incr, MAP(pchar, NodeInfo), iter) != 0) break;
    }
    ITER_FUNC(Destroy, MAP(pchar, NodeInfo), iter);

}

void RemoveEdge_Graph(Graph* g, pchar from, pchar to)
{
    if (HasEdge_Graph(g, from, to) != TRUE)
        return;

    Edge edge = CreateEdge();
    strcpy(edge->from, from);
    strcpy(edge->to, to);

    /* Remove from Edge set */
    SET_FUNC(Remove, Edge, g->edgeSet, edge);

    /* Remove from NodeInfo map */
    ITER(MAP(pchar, NodeInfo))* iter = ITER_FUNC(Create, MAP(pchar, NodeInfo), g->nodeMap);
    while (ITER_FUNC(IsEnd, MAP(pchar, NodeInfo), iter) != TRUE)
    {
        PAIR(pchar, NodeInfo)* pair = ITER_FUNC(Deref, MAP(pchar, NodeInfo), iter);

        NodeInfo info = pair->second;
        SET_FUNC(Remove, Edge, info->outEdges, edge);
        SET_FUNC(Remove, Edge, info->inEdges, edge);

        if (EqualsString(info->theNode, from) == TRUE)
        {
            SET_FUNC(Remove, pchar, info->outNodes, edge->to);
        }
        if (EqualsString(info->theNode, to) == TRUE)
        {
            SET_FUNC(Remove, pchar, info->inNodes, edge->from);
        }

        if (ITER_FUNC(Incr, MAP(pchar, NodeInfo), iter) != 0) break;
    }
    ITER_FUNC(Destroy, MAP(pchar, NodeInfo), iter);
    FreeEdge(edge);
}

bool HasNode_Graph(Graph* g, pchar node)
{
    return MAP_FUNC(HasKey, pchar, NodeInfo, g->nodeMap, node);
}

bool HasEdge_Graph(Graph* g, pchar from, pchar to)
{
    Edge e = CreateEdge();
    strcpy(e->from, from);
    strcpy(e->to, to);

    bool hasEdge = SET_FUNC(Contains, Edge, g->edgeSet, e);
    FreeEdge(e);
    return hasEdge;
}

int InDegree_Graph(Graph* g, pchar node)
{
    if (HasNode_Graph(g, node) != TRUE)
        return 0;

    NodeInfo info = *MAP_FUNC(GetValue, pchar, NodeInfo, g->nodeMap, node);
    return info->inEdges->used;
}

int OutDegree_Graph(Graph* g, pchar node)
{
    if (HasNode_Graph(g, node) != TRUE)
        return 0;

    NodeInfo info = *MAP_FUNC(GetValue, pchar, NodeInfo, g->nodeMap, node);
    return info->outEdges->used;
}

void Print_Graph(Graph* g)
{
    printf("Graph Edges: (prerequisites point to their super course)\n");
    ITER(SET(Edge))* iterEdgeSet = ITER_FUNC(Create, SET(Edge), g->edgeSet);
    while (ITER_FUNC(IsEnd, SET(Edge), iterEdgeSet) != TRUE)
    {
        Edge* graphEdge = ITER_FUNC(Deref, SET(Edge), iterEdgeSet);
        printf("Edge: %s -> %s\n", (*graphEdge)->from, (*graphEdge)->to);
        if (ITER_FUNC(Incr, SET(Edge), iterEdgeSet) != 0) break;
    }
    ITER_FUNC(Destroy, SET(Edge), iterEdgeSet);

    printf("Graph Nodes\n");
    ITER(MAP(pchar, NodeInfo))* iterNodes = ITER_FUNC(Create, MAP(pchar, NodeInfo), g->nodeMap);
    while (ITER_FUNC(IsEnd, MAP(pchar, NodeInfo), iterNodes) != TRUE)
    {
        PAIR(pchar, NodeInfo)* pair = ITER_FUNC(Deref, MAP(pchar, NodeInfo), iterNodes);
        printf("Node: %s\n", pair->first);
        if (ITER_FUNC(Incr, MAP(pchar, NodeInfo), iterNodes) != 0) break;
    }
    ITER_FUNC(Destroy, MAP(pchar, NodeInfo), iterNodes);
}

CoursePlan* InitCoursePlan(int initialNumYears)
{
    CoursePlan* plan = ARRAY_FUNC(Create, PtrArray_PtrSet_pchar, initialNumYears);
    int i = 0;
    for (; i < initialNumYears; ++i)
    {
        ARRAY(PtrSet_pchar)** p = ARRAY_FUNC(At, PtrArray_PtrSet_pchar, plan, i);
        *p = ARRAY_FUNC(Create, PtrSet_pchar, NUM_QTRS);
        int j = 0;
        for (; j < 3; ++j)
        {
            PtrSet_pchar* s = ARRAY_FUNC(At, PtrSet_pchar, *p, j);
            *s = SET_FUNC(Create, pchar, 10, 0.75f);
        }
    }

    return plan;
}

void FreeCoursePlan(CoursePlan* plan)
{
    ARRAY_FUNC(Destroy, PtrArray_PtrSet_pchar, plan);
}

void AddCourse(CoursePlan* plan, int year, int quarter, pchar course)
{
    PtrArray_PtrSet_pchar* vp = ARRAY_FUNC(At, PtrArray_PtrSet_pchar, plan, year);
    PtrSet_pchar* vs = ARRAY_FUNC(At, PtrSet_pchar, *vp, quarter);
    SET_FUNC(Insert, pchar, *vs, course);
}

PtrSet_pchar GetCoursesAt(CoursePlan* plan, int year, int quarter)
{
    PtrArray_PtrSet_pchar* vp = ARRAY_FUNC(At, PtrArray_PtrSet_pchar, plan, year);
    PtrSet_pchar* vs = ARRAY_FUNC(At, PtrSet_pchar, *vp, quarter);
    return *vs;
}

void PrintCoursePlan(CoursePlan* plan)
{
    int year;
    for (year = 0; year < plan->size; ++year)
    {
        int qtr;
        for (qtr = 0; qtr < NUM_QTRS; ++qtr)
        {
            PtrSet_pchar courses = GetCoursesAt(plan, year, qtr);
            if (SET_FUNC(IsEmpty, pchar, courses) != TRUE)
            {
                printf("%i %i", year + 1, qtr + 1);

                ITER(Set_pchar)* iter = ITER_FUNC(Create, Set_pchar, courses);
                while (ITER_FUNC(IsEnd, Set_pchar, iter) != TRUE)
                {
                    printf(" %s", *ITER_FUNC(Deref, Set_pchar, iter));
                    if (ITER_FUNC(Incr, Set_pchar, iter) != 0) break;
                }
                printf("\n");
                ITER_FUNC(Destroy, Set_pchar, iter);
            }

        }
    }
}

void ResizeCoursePlan(CoursePlan* plan, int newSize)
{
    int oldSize = plan->size;
    ARRAY_FUNC(Resize, PtrArray_PtrSet_pchar, plan, newSize);
    int i = oldSize;
    for (; i < plan->size; ++i)
    {
        ARRAY(PtrSet_pchar)** p = ARRAY_FUNC(At, PtrArray_PtrSet_pchar, plan, i);
        *p = ARRAY_FUNC(Create, PtrSet_pchar, NUM_QTRS);
        int j = 0;
        for (; j < 3; ++j)
        {
            PtrSet_pchar* s = ARRAY_FUNC(At, PtrSet_pchar, *p, j);
            *s = SET_FUNC(Create, pchar, 10, 0.75f);
        }
    }
}

void ReadRequiredCourses(FILE* reqFile, Graph* courseGraph, SET(pchar)* outReqSet)
{
    char course[MAX_COURSENAME_SIZE];

    while (fscanf(reqFile, "%s", course) != EOF)
    {
        AddNode_Graph(courseGraph, course);
        SET_FUNC(Insert, pchar, outReqSet, course);
    }
}

void ReadPrerequisites(FILE* prereqFile, Graph* g)
{
    char keyCourse[MAX_COURSENAME_SIZE];
    char prereqCourse[MAX_COURSENAME_SIZE];

    for (;; )
    {
        /*read the first course name on the line, stopping if EOF is reached*/
        if (fscanf(prereqFile, "%s", prereqCourse) == EOF)
        {
            break;
        };
        strcpy(keyCourse, prereqCourse);
        AddNode_Graph(g, keyCourse); /* Add the key course (one with prereqs) to graph*/

        if (SkipWhitespace(prereqFile) == -1) /* if EOF or newline, continue*/
        {
            continue;
        }

        while (fscanf(prereqFile, "%s", prereqCourse) != EOF)
        {
            /* Add the prereq to the graph, make edge from prereq to the course requiring it*/
            AddNode_Graph(g, prereqCourse);
            AddEdge_Graph(g, prereqCourse, keyCourse);

            if (SkipWhitespace(prereqFile) == -1) /*stop adding courses as a prereq if newline or EOF is found*/
            {
                break;
            }
        }
    }
}

void ReadOfferedCourses(FILE* offeringFile, SET(pchar)* necessaryCourses, SET(pchar)* reqCourses, SET(pchar)** outOfferedCourses)
{
    char course[MAX_COURSENAME_SIZE];
    int quarter = 0;

    while (fscanf(offeringFile, "%s", course) != EOF && fscanf(offeringFile, "%i", &quarter) != EOF)
    {
        if (SET_FUNC(Contains, pchar, necessaryCourses, course) == TRUE || SET_FUNC(Contains, pchar, reqCourses, course) == TRUE)
        {
            SET_FUNC(Insert, pchar, outOfferedCourses[quarter - 1], course); /* Get only the required courses for graduation */
        }
    }
}

SET(pchar)* GetAllNecessaryCourses(Graph* g, SET(pchar)* reqSet)
{
    EquivalenceClass* ec = CreateEquivalenceClass();
    /* Use an equivalence class to get reachability from all required courses */
    SET(Edge)* edges = g->edgeSet;
    ITER(Set_Edge)* iter = ITER_FUNC(Create, Set_Edge, edges);
    while (ITER_FUNC(IsEnd, Set_Edge, iter) != TRUE)
    {
        /* For each edge in graph, add both "from" and "to" to the EquivalenceClass, and
         * then merge their classes if "to" is required.
         */
        Edge edge = *ITER_FUNC(Deref, Set_Edge, iter);
        AddElement(ec, edge->from);
        AddElement(ec, edge->to);
        if (SET_FUNC(Contains, pchar, reqSet, edge->to) == TRUE)
            MergeClasses(ec, edge->from, edge->to);

        if (ITER_FUNC(Incr, Set_Edge, iter) != 0) break;
    }
    ITER_FUNC(Destroy, Set_Edge, iter);

    ITER(Set_pchar)* reqIter = ITER_FUNC(Create, Set_pchar, reqSet);
    pchar first = *ITER_FUNC(Deref, Set_pchar, reqIter); /* arbitrarily pick the first one to merge with */
    if (ITER_FUNC(Incr, Set_pchar, reqIter) != 0) return NULL;
    while (ITER_FUNC(IsEnd, Set_pchar, reqIter) != TRUE)
    {
        /* For each required course, merge their equivalence classes */
        MergeClasses(ec, first, *ITER_FUNC(Deref, Set_pchar, reqIter));

        if (ITER_FUNC(Incr, Set_pchar, reqIter) != 0) break;
    }
    ITER_FUNC(Destroy, Set_pchar, reqIter);

    SET(pchar)* necessaryCourses = GetClassOf(ec, first);

    DestroyEquivalenceClass(ec);

    return necessaryCourses;
}

void TakeCourse(pchar course, SET(pchar)* requiredCourses, SET(pchar)* necessaryCourses, int currentYear, int currentQuarter, CoursePlan* outPlan)
{
    AddCourse(outPlan, currentYear, currentQuarter, course);
    SET_FUNC(Remove, pchar, necessaryCourses, course);
    SET_FUNC(Remove, pchar, requiredCourses, course);
}

void UpdatePrerequisites(Graph* courseGraph, SET(pchar)* prereqsTaken)
{
    /* For all pending prereqs, remove the node from the graph */
    ITER(Set_pchar)* iter = ITER_FUNC(Create, Set_pchar, prereqsTaken);
    while (ITER_FUNC(IsEnd, Set_pchar, iter) != TRUE)
    {
        pchar courseTaken = *ITER_FUNC(Deref, Set_pchar, iter);
        RemoveNode_Graph(courseGraph, courseTaken);
        ITER_FUNC(Incr, Set_pchar, iter);
    }
    ITER_FUNC(Destroy, Set_pchar, iter);
}

void CreateCoursePlan(FILE* offeringFile, FILE* prereqFile, FILE* reqFile, CoursePlan* outPlan)
{
    Graph* courseGraph = Create_Graph();

    SET(pchar)* reqCourses = SET_FUNC(Create, pchar, 10, 0.75f);
    ReadRequiredCourses(reqFile, courseGraph, reqCourses);
    fclose(reqFile);

    ReadPrerequisites(prereqFile, courseGraph);
    fclose(prereqFile);

    SET(pchar)* necessaryCourses = GetAllNecessaryCourses(courseGraph, reqCourses);

    SET(pchar)* offeredCourses[NUM_QTRS];
    int i = 0;
    for (i = 0; i < NUM_QTRS; ++i)
    {
        offeredCourses[i] = SET_FUNC(Create, pchar, 10, 0.75f);
    }

    ReadOfferedCourses(offeringFile, necessaryCourses, reqCourses, offeredCourses);
    fclose(offeringFile);

    SET(pchar)* prereqsTaken = SET_FUNC(Create, pchar, 10, 0.75f);

    int currentYear = 0;
    int currentQuarter = FALL;

    //SET( pchar )* sourceNodes = SET_FUNC( Create, pchar, 10, 0.75f );
    //ITER( Map_pchar_NodeInfo )* nodeIter = ITER_FUNC( Create, Map_pchar_NodeInfo, courseGraph->nodeMap );
    //while ( ITER_FUNC( IsEnd, Map_pchar_NodeInfo, nodeIter ) != TRUE )
    //{
    //    NodeInfo nodeInfo = ITER_FUNC( Deref, Map_pchar_NodeInfo, nodeIter )->second;
    //    if ( nodeInfo->inEdges->used == 0 )
    //    {
    //        SET_FUNC( Insert, pchar, sourceNodes, nodeInfo->theNode );
    //    }
    //    if ( ITER_FUNC( Incr, Map_pchar_NodeInfo, nodeIter ) != 0 ) break;
    //}

    /* Take all required courses */
    while (SET_FUNC(IsEmpty, pchar, necessaryCourses) == FALSE && SET_FUNC(IsEmpty, pchar, reqCourses) == FALSE)
    {
        SET(pchar)* coursesThisQtr = offeredCourses[currentQuarter]; /* will contain only necessary courses that are in this quarter */
        ITER(Set_pchar)* courseIter = ITER_FUNC(Create, Set_pchar, coursesThisQtr);
        while (ITER_FUNC(IsEnd, Set_pchar, courseIter) != TRUE)
        {
            /* go through all required courses offered this quarter */
            pchar course = *ITER_FUNC(Deref, Set_pchar, courseIter);
            int inDegree = InDegree_Graph(courseGraph, course);
            if (inDegree == 0)
            {
                /* no prereqs, take course if it's not in the same quarter as its prereq */
                TakeCourse(course, reqCourses, necessaryCourses, currentYear, currentQuarter, outPlan);
                SET_FUNC(Insert, pchar, prereqsTaken, course);
                /* also remove it from the courses for this quarter, since now it's taken */
                ITER_FUNC(RemoveCurrent, Set_pchar, courseIter);
            }

            if (ITER_FUNC(Incr, Set_pchar, courseIter) != 0) break;
        }
        ITER_FUNC(Destroy, Set_pchar, courseIter);

        /* At the end of each quarter, remove the nodes of the courses taken that quarter */
        UpdatePrerequisites(courseGraph, prereqsTaken);
        /* Destroy and recreate the set */
        SET_FUNC(Destroy, pchar, prereqsTaken);
        prereqsTaken = SET_FUNC(Create, pchar, 10, 0.75f);

        ++currentQuarter;
        if (currentQuarter >= NUM_QTRS)
        {
            ++currentYear;
            if (currentYear >= outPlan->size)
            {
                ResizeCoursePlan(outPlan, outPlan->size * 2);
            }
            currentQuarter = FALL;
        }
    }

    for (i = 0; i < NUM_QTRS; ++i)
    {
        SET_FUNC(Destroy, pchar, offeredCourses[i]);
    }

    SET_FUNC(Destroy, pchar, prereqsTaken);
    Destroy_Graph(courseGraph);
    SET_FUNC(Destroy, pchar, reqCourses);
    SET_FUNC(Destroy, pchar, necessaryCourses);
}

int SkipWhitespace(FILE* file)
{
    for (;; )
    {
        /* read the next characters in the file, discarding
          whitespace and stopping when end of line or
          non-whitespace is found */
        int c = fgetc(file);
        if (c != EOF)
        {
            if (c == '\r')
            {
                fgetc(file); /* get another character, the \n */
                return -1;
            }
            else if (c == '\n')
            {
                return -1;
            }
            /*check for whitespace & tabs (note - isspace also returns nonzero for \r and \n but these are handled separately)*/
            else if (isspace(c))
            {
                continue;
            }
            else
            {
                ungetc(c, file);
                return 0;
            }
        }
        else
        {
            return -1;
        }
    }

    return 1;
}


EquivalenceClass* CreateEquivalenceClass()
{
    EquivalenceClass* ec = malloc(sizeof(struct EquivalenceClass));
    ec->parentMap = MAP_FUNC(Create, pchar, pchar, 30, 0.75f);
    ec->rootSizeMap = MAP_FUNC(Create, pchar, int, 30, 0.75f);
    return ec;
}

void DestroyEquivalenceClass(EquivalenceClass* ec)
{
    if (ec)
    {
        MAP_FUNC(Destroy, pchar, int, ec->rootSizeMap);
        MAP_FUNC(Destroy, pchar, pchar, ec->parentMap);
        free(ec);
    }
}

void AddElement(EquivalenceClass* ec, pchar c)
{
    /* Do nothing if already in the parent map */
    if (MAP_FUNC(HasKey, pchar, pchar, ec->parentMap, c) == TRUE)
    {
        return;
    }

    /*  Get parent entry, copy given string into the entry string */
    pchar* p = MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, c);
    strcpy(*p, c);
    /* Update the size to 1 */
    int* size = MAP_FUNC(GetValue, pchar, int, ec->rootSizeMap, c);
    *size = 1;
}

void MergeClasses(EquivalenceClass* ec, pchar c1, pchar c2)
{
    /* Do nothing if both are not in the parent map*/
    if (MAP_FUNC(HasKey, pchar, pchar, ec->parentMap, c1) == TRUE && MAP_FUNC(HasKey, pchar, pchar, ec->parentMap, c2) == TRUE)
    {
        pchar rootC1 = CompressToRoot(ec, c1);
        pchar rootC2 = CompressToRoot(ec, c2);

        if (EqualsString(rootC1, rootC2) != TRUE)
        {
            int* sizeRootC1 = MAP_FUNC(GetValue, pchar, int, ec->rootSizeMap, c1);
            int* sizeRootC2 = MAP_FUNC(GetValue, pchar, int, ec->rootSizeMap, c2);

            /* Set the parent of the smaller root to the larger one and add its size
               to the size of the larger one.*/
            if (*sizeRootC1 > *sizeRootC2)
            {
                pchar* parentRootC2 = MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, rootC2); /* get the parent entry string */
                strcpy(*parentRootC2, rootC1); /* make parent2 = root1*/
                *sizeRootC1 += *sizeRootC2;
                MAP_FUNC(Remove, pchar, int, ec->rootSizeMap, rootC2);
            }
            else
            {
                pchar* parentRootC1 = MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, rootC1); /* get the parent entry string */
                strcpy(*parentRootC1, rootC2); /* make parent1 = root2*/
                *sizeRootC2 += *sizeRootC1;
                MAP_FUNC(Remove, pchar, int, ec->rootSizeMap, rootC1);
            }
        }

        FreeString(rootC1);
        FreeString(rootC2);
    }
}

bool InSameClass(EquivalenceClass* ec, pchar c1, pchar c2)
{
    /* Only compare classes if both given are in the parent map */
    if (MAP_FUNC(HasKey, pchar, pchar, ec->parentMap, c1) == TRUE && MAP_FUNC(HasKey, pchar, pchar, ec->parentMap, c2) == TRUE)
    {
        pchar rootC1;
        for (rootC1 = *MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, c1);
            EqualsString(rootC1, *MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, rootC1)) != TRUE;
            rootC1 = *MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, rootC1))
        {
        }

        pchar rootC2;
        for (rootC2 = *MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, c2);
            EqualsString(rootC2, *MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, rootC1)) != TRUE;
            rootC1 = *MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, rootC2))
        {
        }

        return EqualsString(rootC1, rootC2);
    }

    return FALSE;
}

SET(pchar)* GetClassOf(EquivalenceClass* ec, pchar c)
{
    if (MAP_FUNC(HasKey, pchar, pchar, ec->parentMap, c) == FALSE)
    {
        return NULL;
    }

    /* Compress every value to its root */
    ITER(Map_pchar_pchar)* parentIter = ITER_FUNC(Create, Map_pchar_pchar, ec->parentMap);
    while (ITER_FUNC(IsEnd, Map_pchar_pchar, parentIter) != TRUE)
    {
        pchar root = CompressToRoot(ec, ITER_FUNC(Deref, Map_pchar_pchar, parentIter)->first);
        FreeString(root);
        if (ITER_FUNC(Incr, Map_pchar_pchar, parentIter) != 0) break;
    }
    ITER_FUNC(Destroy, Map_pchar_pchar, parentIter);

    /* Get root of C */
    pchar rootC = CompressToRoot(ec, c);

    /* Get the equivalence set that contains the given value c.*/
    SET(pchar)* eqvSetWithC = SET_FUNC(Create, pchar, 20, 0.75f);
    parentIter = ITER_FUNC(Create, Map_pchar_pchar, ec->parentMap);
    while (ITER_FUNC(IsEnd, Map_pchar_pchar, parentIter) != TRUE)
    {
        PAIR(pchar, pchar)* valAndRoot = ITER_FUNC(Deref, Map_pchar_pchar, parentIter);
        if (EqualsString(valAndRoot->second, rootC) == TRUE)
        {
            SET_FUNC(Insert, pchar, eqvSetWithC, valAndRoot->first);
        }
        if (ITER_FUNC(Incr, Map_pchar_pchar, parentIter) != 0) break;
    }
    ITER_FUNC(Destroy, Map_pchar_pchar, parentIter);
    FreeString(rootC);
    return eqvSetWithC;
}

pchar CompressToRoot(EquivalenceClass* ec, pchar c)
{
    pchar value = DefaultString();

    SET(pchar)* ancestors = SET_FUNC(Create, pchar, 20, 0.75f);
    SET_FUNC(Insert, pchar, ancestors, c);

    /* Find all children */
    for (strcpy(value, *MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, c));
        EqualsString(*MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, value), value) != TRUE;
        strcpy(value, *MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, value)))
    {
        SET_FUNC(Insert, pchar, ancestors, value);
    }

    ITER(Set_pchar)* ancestorIter = ITER_FUNC(Create, Set_pchar, ancestors);
    while (ITER_FUNC(IsEnd, Set_pchar, ancestorIter) != TRUE)
    {
        /* Make each ancestor's parent the new root*/
        pchar* parentOfAncestor = MAP_FUNC(GetValue, pchar, pchar, ec->parentMap, *ITER_FUNC(Deref, Set_pchar, ancestorIter));
        strcpy(*parentOfAncestor, value);
        if (ITER_FUNC(Incr, Set_pchar, ancestorIter) != 0) break;
    }
    ITER_FUNC(Destroy, Set_pchar, ancestorIter);
    SET_FUNC(Destroy, pchar, ancestors);
    return value;
}
