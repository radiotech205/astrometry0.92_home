/*
 # This file is part of the Astrometry.net suite.
 # Licensed under a 3-clause BSD style license - see LICENSE
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "boilerplate.h"
#include "solvedfile.h"
#include "an-bool.h"
#include "memory.h"
const char* OPTIONS_mergesolved = "ho:e";

void printHelp_mergesolved(char* progname) {
    BOILERPLATE_HELP_HEADER(stderr);
    fprintf(stderr, "\nUsage: %s -o <output-file> <input-file> ...\n"
            "    [-e]: no error if file no found (assume empty)\n"
            "\n", progname);
}


int main_mergesolved(int argc, char** args) {
    int argchar;
    char* progname = args[0];
    char** inputfiles = NULL;
    int ninputfiles = 0;
    int i;
    char* outfile = NULL;
    int N;
    anbool* solved;
    int noerr = 0;

    while ((argchar = getopt (argc, args, OPTIONS_mergesolved)) != -1) {
        switch (argchar) {
        case 'o':
            outfile = optarg;
            break;
        case 'e':
            noerr = 1;
            break;
        case 'h':
        default:
            printHelp_mergesolved(progname);
            exit(-1);
        }
    }
    if (optind < argc) {
        ninputfiles = argc - optind;
        inputfiles = args + optind;
    } else {
        printHelp_mergesolved(progname);
        exit(-1);
    }

    N = 0;
    for (i=0; i<ninputfiles; i++) {
        int n = solvedfile_getsize(inputfiles[i]);
        if (n == -1) {
            if (!noerr) {
                fprintf(stderr, "Failed to get size of input file %s.\n", inputfiles[i]);
                exit(-1);
            }
        }
        if (n > N) N = n;
    }
    solved = smart_calloc(N, sizeof(anbool));
    if(!solved) return 0;
    for (i=0; i<ninputfiles; i++) {
        il* slist;
        int j;
        slist = solvedfile_getall_solved(inputfiles[i], 1, N, 0);
        for (j=0; j<il_size(slist); j++)
            solved[il_get(slist, j) - 1] = TRUE;
        il_free(slist);
    }
    if (solvedfile_set_file(outfile, solved, N)) {
        fprintf(stderr, "Failed to set values in output file.\n");
        exit(-1);
    }

    smart_free(solved);
    return 0;
}
