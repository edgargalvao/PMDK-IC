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

#include <immer/map.hpp>

#define BUILD_STD_MAP       1
#define BUILD_NVM_MAP       2
#define SUM_STD_MAP         3
#define SUM_NVM_MAP         4
#define ERASE_STD_MAP       5
#define ERASE_NVM_MAP       6

#define INSERT 1
#define UPDATE 2

using map_type = immer::map<int, int>;

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
void do_bench(FILE *stream, int bench, int op, size_t start, size_t end,
              size_t tick, char *path)
{
    for (size_t n = start; n <= end; n += tick)
    {
        nvm_initialize(path, 0);
        map_type *nvm_map = new map_type();
        std::map<int, int> *std_map = new std::map<int, int>();

        int sum0 = (int)(((long long)n - 1) * (long long)n / 2); 
        unsigned int count_expected = 0;
            
        fprintf(stream, "%zu ", n);

        switch (bench)
        {
            case BUILD_STD_MAP:
            {
                size_t t0 = get_time();
                for (int i = 0; (unsigned int) i < n; i++)
                    std_map->insert(std::make_pair(i, i));
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                break;
            }
            case SUM_STD_MAP:
            {
                for (int i = 0; (unsigned int) i < n; i++)
                    std_map->insert(std::make_pair(i, i));
                int sum = 0;
                size_t t0 = get_time();
                for (auto x: *std_map)
                    sum += x.second;
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                assert(sum == sum0);
                break;
            }
            case ERASE_STD_MAP:
            {
                for (int i = 0; (unsigned int) i < n; i++)
                    std_map->insert(std::make_pair(i, i));
                size_t t0 = get_time();
                for (int i = 0;  (unsigned int) i < n; i++)
                    std_map->erase(i);
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                assert(std_map->size() == 0);
                break;
            }
            case BUILD_NVM_MAP:
            {
                size_t t0 = get_time();
                if (op == INSERT) {
                    for (int i = 0;  (unsigned int) i < n; i++) {
                        nvm_map = nvm_map->set_ptr(i, i);
                    }
                    count_expected = n;
                } else if (op == UPDATE) {
                    for (int i = 0;  (unsigned int) i < n; i++) {
                        nvm_map = nvm_map->set_ptr(0, i);
                    }
                    count_expected = 1;
                }
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                assert(nvm_map->size() == count_expected);
                break;
            }
            case SUM_NVM_MAP:
            {
            }
            case ERASE_NVM_MAP:
            {
                for (int i = 0;  (unsigned int) i < n; i++) {
                    nvm_map = nvm_map->set_ptr(i, i);
                }
                size_t t0 = get_time();
                for (int i = 0; (unsigned int) i < n; i++) {
                    nvm_map = nvm_map->erase_ptr(i);
                }
                size_t t1 = get_time();
                fprintf(stream, "%zu\n", t1 - t0);
                assert(nvm_map->size() == 0);
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
    if (argc != 7)
    {
        fprintf(stderr, "usage: %s bench op start end tick backing-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int bench = 0;
    if (strcmp(argv[1], "build_std_map") == 0)
        bench = BUILD_STD_MAP;
    else if (strcmp(argv[1], "build_nvm_map") == 0)
        bench = BUILD_NVM_MAP;
    else if (strcmp(argv[1], "sum_std_map") == 0)
        bench = SUM_STD_MAP;
    else if (strcmp(argv[1], "erase_std_map") == 0)
        bench = ERASE_STD_MAP;
    else if (strcmp(argv[1], "erase_nvm_map") == 0)
        bench = ERASE_NVM_MAP;
    else
    {
        fprintf(stderr, "error: bad benchmark \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int op = 0;
    if (strcmp(argv[2], "insert") == 0)
        op = INSERT;
    else if (strcmp(argv[2], "update") == 0)
        op = UPDATE;
    else
    {
        fprintf(stderr, "error: bad operation\"%s\"\n", argv[2]);
        exit(EXIT_FAILURE);
    }


    size_t start = atoll(argv[3]);
    size_t end   = atoll(argv[4]);
    size_t tick  = atoll(argv[5]);
    char *path = argv[6];

    do_bench(stdout, bench, op, start, end, tick, path);

    return 0;
}

