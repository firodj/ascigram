#ifdef _WIN32
/**
 * https://github.com/mattn/ansicolor-w32.c
 * MIT License
 * Yasuhiro Matsumoto (a.k.a mattn)
 */
#define fprintf(...) _fprintf_w32(__VA_ARGS__);
#define printf(...) _fprintf_w32(stdout, __VA_ARGS__);
#define fputs(fp, x) _fprintf_w32(fp, x);
#define puts(x) _fprintf_w32(stdout, x);
#endif
