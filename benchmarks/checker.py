#!/usr/bin/env python

import sys
import os
import json
from sets import Set

def main(argv):
    filename = argv[0];
    lines_modified = Set();
    refcounts_modified = Set();
    lines_flushed = Set();
    outfilename = filename.split('.')[0] + '_checked.log'
    out = open(outfilename, "w")
    analyze = False;
    total_blocks = 0;
    incorrect_blocks = 0;
    expensive_stores = 0;
    extra_flushes = 0;
    with open(filename) as tracefile:
        trace = tracefile.readlines();  
        for line in trace:
            # There is messy output when VALGRIND starts instrumenting
            # so we skip ahead to first FENCE.
            if not analyze:
                if line.startswith("FENCE"):
                    analyze = True;
                continue;
            if line.startswith("STORE "):
                addr = int(line.split(' ')[1], 16);
                size = int(line.split(' ')[2]);
                cl_addr = hex(addr - (addr%64));
                lines_modified.add(cl_addr);
                end_addr = addr + size - 1;
                end_cl_addr = hex(end_addr - (end_addr%64));
                if end_cl_addr > cl_addr:
                    lines_modified.add(end_cl_addr);
                if cl_addr in lines_flushed or end_cl_addr in lines_flushed:
                    expensive_stores += 1;
                    line = line.rstrip("\n\r");
                    line += " [Expensive]\n"
            elif line.startswith("FLUSH "):
                addr = int(line.split(' ')[1], 16)
                cl_addr = hex(addr - (addr%64));
                lines_flushed.add(cl_addr);
                if cl_addr not in lines_modified:
                    line = line.rstrip("\n\r");
                    line += " [Extra Flush]\n";
                    extra_flushes += 1;
                else:
                    lines_modified.discard(cl_addr);
            elif "VOLATILE REF" in line:
                addr = int(line.split(':')[1], 16);
                # Convert address to cacheline address
                refcounts_modified.add(hex(addr - (addr%64)));
            elif "FENCE" in line:
                total_blocks += 1;
                # Do not track volatile refcounts
                for addr in refcounts_modified:
                    lines_modified.discard(addr)
                if (len(lines_modified) > 0):
                    incorrect_blocks += 1;
                    line += "Unflushed addresses:";
                    if(len(lines_modified) == 1):
                        line+= " [Address may be a refcount, which are volatile but stored in PM for simplicity]\n";
                    line += str(lines_modified) + "\n";
                lines_modified.clear();
                lines_flushed.clear();
                refcounts_modified.clear();
            out.write(line);
    result = "Extra Flushes:" + str(extra_flushes) + "\n";
    result += "Expensive Stores:" + str(expensive_stores) + "\n";
    result += str(incorrect_blocks) + "/" + str(total_blocks) + " blocks found to be incorrect.";
    out.write(result);
    print(result);

if __name__ == "__main__":
        main(sys.argv[1:])
