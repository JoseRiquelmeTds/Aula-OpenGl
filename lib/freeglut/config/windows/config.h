/* freeglut — configuração para Windows com MinGW-w64 (MSYS2), usada pelo Makefile do projeto.
 *
 * Equivale ao config.h que o CMake do freeglut 3.8.0 gera com MinGW-w64.
 */

#define FREEGLUT_PRINT_WARNINGS
#define FREEGLUT_PRINT_ERRORS

#define HAVE_SYS_TYPES_H
#define HAVE_UNISTD_H
#define HAVE_SYS_TIME_H
#define HAVE_STDBOOL_H
#define HAVE_SYS_PARAM_H
#define HAVE_FCNTL_H
#define HAVE_ERRNO_H
#define HAVE_GETTIMEOFDAY
#define HAVE_VFPRINTF
#define HAVE_STDINT_H
#define HAVE_INTTYPES_H
#define HAVE_ULONG_LONG

/* Sem X11 no Windows: usa a implementação própria em src/util/xparsegeometry_repl.c */
#define NEED_XPARSEGEOMETRY_IMPL
#undef HAVE_U__INT64
