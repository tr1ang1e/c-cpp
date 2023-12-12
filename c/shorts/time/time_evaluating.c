
#ifdef COUNT_TIME_PROC

    #define LYELL                          "\x1B[38;5;222m"
    #define __time_start_proc(var)          printf(GREY "[  TIME ]   " LINE "( %s )\n" ORIG, __LINE__, var);
    #define __time_color_proc(var, res)     printf(GREY "[  TIME ]   " LINE "( %s ): " LYELL "%f\n" ORIG, __LINE__, var, res);
    #define __TStaP(var)                    clock_t var; __time_start_proc(#var); var = clock();
    #define __TEndP(var)                    var = clock() - var; \
                                            double res##var = (((double)var)/CLOCKS_PER_SEC) * 1000000; \
                                            __time_color_proc(#var, res##var)

#else

    #define __TStaP(var)
    #define __TEndP(var)

#endif // COUNT_TIME_PROC





#ifdef COUNT_TIME_REAL

    #define PURP                          "\x1B[38;5;90m"
    #define __time_start_real(var)          printf(GREY "[  TIME ]   " LINE "( %s )\n" ORIG, __LINE__, var);
    #define __time_color_real(var, res)     printf(GREY "[  TIME ]   " LINE "( %s ): " PURP "%lu\n" ORIG, __LINE__, var, res);
    #define __TStaR(var)                    struct timespec s##var, e##var; __time_start_real(#var); clock_gettime(CLOCK_MONOTONIC_RAW, &s##var);
    #define __TEndR(var)                    clock_gettime(CLOCK_MONOTONIC_RAW, &e##var); \
                                            uint64_t res##var = ((e##var.tv_sec - s##var.tv_sec) * 1000000 + (e##var.tv_nsec - s##var.tv_nsec) / 1000); \
                                            __time_color_real(#var, res##var);

#else
    #define __TStaR(var)
    #define __TEndR(var)

#endif // COUNT_TIME_REAL
