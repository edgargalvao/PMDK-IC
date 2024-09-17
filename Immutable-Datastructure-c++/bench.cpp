/*
 * Copyright (c) 2017 The National University of Singapore.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <cerrno>
#include <cassert>
#include <cstdio>
#include <ctime>
#include <cstring>

#include <vector>
#include <map>

#include "immer/vector.hpp"

#define BUILD_STD_VECTOR    4
#define BUILD_STD_MAP       5
#define SUM_STD_VECTOR      12
#define SUM_STD_MAP         13
#define BUILD_IMMER_VECTOR  14

/*
 * Get the time in milliseconds.
 */
static uint64_t get_time(void)
{
    // Linux:
    struct timespec ts;
    unsigned tick = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    tick  = ts.tv_nsec / 1000000;
    tick += ts.tv_sec * 1000;
    return tick;
}

/*
 * Run the benchmark.
 */
void do_bench(FILE *stream, int bench, size_t start, size_t end, size_t tick)
{
    for (size_t n = start; n <= end; n += tick)
    {
        std::vector<int> s;
//        immer::vector<int> u = immer::vector<int>{};
        std::map<int, int> m;
        uint64_t t0, t1;

        int sum0 = (int)(((long long)n - 1) * (long long)n / 2); 

        fprintf(stream, "%zu ", n);

        switch (bench)
        {
            case BUILD_STD_VECTOR:
            {
                size_t t0 = get_time();
                for (int i = 0; i < n; i++)
                    s.push_back(i);
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                break;
            }
            case BUILD_IMMER_VECTOR:
            {
/*                size_t t0 = get_time();
                for (int i = 0; i < n; i++)
                    u.push_back(i);
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                break;
                */
            }
            case BUILD_STD_MAP:
            {
                size_t t0 = get_time();
                for (int i = 0; i < n; i++)
                    m.insert(std::make_pair(i, i));
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                break;
            }
            case SUM_STD_VECTOR:
            {
               for (int i = 0; i < n; i++)
                    s.push_back(i);
                int sum = 0;
                size_t t0 = get_time();
                for (auto x: s)
                    sum += x;
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                assert(sum == sum0);
                break;
            }
            case SUM_STD_MAP:
            {
                for (int i = 0; i < n; i++)
                    m.insert(std::make_pair(i, i));
                int sum = 0;
                size_t t0 = get_time();
                for (auto x: m)
                    sum += x.second;
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                assert(sum == sum0);
                break;
            }
            default:
                fprintf(stderr, "error: unknown bench (%d)\n", bench);
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        fprintf(stderr, "usage: %s bench start end tick\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int bench = 0;
    if (strcmp(argv[1], "build_std_vector") == 0)
        bench = BUILD_STD_VECTOR;
    else if (strcmp(argv[1], "build_std_map") == 0)
        bench = BUILD_STD_MAP;
    else if (strcmp(argv[1], "build_immer_vector") == 0)
        bench = BUILD_IMMER_VECTOR;
    else if (strcmp(argv[1], "sum_std_vector") == 0)
        bench = SUM_STD_VECTOR;
    else if (strcmp(argv[1], "sum_std_map") == 0)
        bench = SUM_STD_MAP;
    else
    {
        fprintf(stderr, "error: bad benchmark \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    size_t start = atoll(argv[2]);
    size_t end   = atoll(argv[3]);
    size_t tick  = atoll(argv[4]);

    do_bench(stdout, bench, start, end, tick);     // Real run.

    return 0;
}

