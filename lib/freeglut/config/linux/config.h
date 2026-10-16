/* freeglut — configuração para Linux/WSL (X11 + GLX), usada pelo Makefile do projeto.
 *
 * Baseado em altbuild/config.h.unix do freeglut 3.8.0. As extensões opcionais do X11
 * (XRandR, XF86VidMode, XInput2) estão desligadas para o build só precisar de libX11:
 * elas servem para trocar a resolução no game mode e para joystick/multitouch.
 */

#undef HAVE_X11_EXTENSIONS_XF86VMODE_H
#undef HAVE_X11_EXTENSIONS_XRANDR_H
#undef HAVE_X11_EXTENSIONS_XINPUT2_H
#undef HAVE_USBHID_H

#define FREEGLUT_PRINT_WARNINGS
#define FREEGLUT_PRINT_ERRORS

#define HAVE_SYS_TYPES_H
#define HAVE_UNISTD_H
#define HAVE_SYS_TIME_H
#define HAVE_STDBOOL_H
#define HAVE_SYS_PARAM_H
#define HAVE_SYS_IOCTL_H
#define HAVE_FCNTL_H
#define HAVE_ERRNO_H
#define HAVE_GETTIMEOFDAY
#define HAVE_VFPRINTF
#define HAVE_STDINT_H
#define HAVE_INTTYPES_H
#define HAVE_ULONG_LONG

/* XParseGeometry vem da libX11 */
#undef NEED_XPARSEGEOMETRY_IMPL
#undef HAVE_U__INT64
