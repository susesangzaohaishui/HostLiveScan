#ifndef DEBUG_H
#define DEBUG_H
#ifdef __DEBUG__
#define debug_str(...)  fprintf (stdout, __VA_ARGS__)
#define debug_err(...) fprintf (stderr, __VA_ARGS__)
#define debug_fun(...) printf("[lch]:File:%s, Line:%d, Function:%s\n" \
     __VA_ARGS__, __FILE__, __LINE__ ,__FUNCTION__);
#else
#define debug_str(...)
#define debug_err(...)
#define debug_fun(...)
#endif

#endif // DEBUG_H
