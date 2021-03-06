﻿/*
 * Copyright (c) 2008-2010 Stefano Sabatini
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <unistd.h>             /* getopt */

#undef HAVE_AV_CONFIG_H
#include "libavutil/pixdesc.h"
#include "libavfilter/graphparser.h"

static void usage(void)
{
    printf("Convert a libavfilter graph to a dot file\n");
    printf("Usage: graph2dot [OPTIONS]\n");
    printf("\n"
           "Options:\n"
           "-i INFILE         set INFILE as input file, stdin if omitted\n"
           "-o OUTFILE        set OUTFILE as output file, stdout if omitted\n"
           "-h                print this help\n");
}

struct line
{
    char data[256];
    struct line *next;
};

static void print_digraph(FILE *outfile, AVFilterGraph *graph)
{
    int i, j;

    fprintf(outfile, "digraph G {\n");
    fprintf(outfile, "node [shape=box]\n");
    fprintf(outfile, "rankdir=LR\n");

    for (i = 0; i < graph->filter_count; i++)
    {
        char filter_ctx_label[128];
        const AVFilterContext *filter_ctx = graph->filters[i];

        snprintf(filter_ctx_label, sizeof(filter_ctx_label), "%s (%s)",
                 filter_ctx->name,
                 filter_ctx->filter->name);

        for (j = 0; j < filter_ctx->output_count; j++)
        {
            AVFilterLink *link = filter_ctx->outputs[j];
            if (link)
            {
                char dst_filter_ctx_label[128];
                const AVFilterContext *dst_filter_ctx = link->dst;

                snprintf(dst_filter_ctx_label, sizeof(dst_filter_ctx_label), "%s (%s)",
                         dst_filter_ctx->name,
                         dst_filter_ctx->filter->name);

                fprintf(outfile, "\"%s\" -> \"%s\"", filter_ctx_label, dst_filter_ctx_label);
                fprintf(outfile, " [ label= \"fmt:%s w:%d h:%d\"];\n",
                        av_pix_fmt_descriptors[link->format].name, link->w, link->h);
            }
        }
    }
    fprintf(outfile, "}\n");
}

int main(int argc, char **argv)
{
    const char *outfilename = NULL;
    const char *infilename = NULL;
    FILE *outfile = NULL;
    FILE *infile = NULL;
    char *graph_string = NULL;
    AVFilterGraph *graph = av_mallocz(sizeof(AVFilterGraph));
    char c;

    av_log_set_level(AV_LOG_DEBUG);

    while ((c = getopt(argc, argv, "hi:o:")) != -1)
    {
        switch(c)
        {
        case 'h':
            usage();
            return 0;
        case 'i':
            infilename = optarg;
            break;
        case 'o':
            outfilename = optarg;
            break;
        case '?':
            return 1;
        }
    }

    if (!infilename || !strcmp(infilename, "-"))
        infilename = "/dev/stdin";
    infile = fopen(infilename, "r");
    if (!infile)
    {
        fprintf(stderr, "Impossible to open input file '%s': %s\n", infilename, strerror(errno));
        return 1;
    }

    if (!outfilename || !strcmp(outfilename, "-"))
        outfilename = "/dev/stdout";
    outfile = fopen(outfilename, "w");
    if (!outfile)
    {
        fprintf(stderr, "Impossible to open output file '%s': %s\n", outfilename, strerror(errno));
        return 1;
    }

    /* read from infile and put it in a buffer */
    {
        unsigned int count = 0;
        struct line *line, *last_line, *first_line;
        char *p;
        last_line = first_line = av_malloc(sizeof(struct line));

        while (fgets(last_line->data, sizeof(last_line->data), infile))
        {
            struct line *new_line = av_malloc(sizeof(struct line));
            count += strlen(last_line->data);
            last_line->next = new_line;
            last_line = new_line;
        }
        last_line->next = NULL;

        graph_string = av_malloc(count + 1);
        p = graph_string;
        for (line = first_line; line->next; line = line->next)
        {
            unsigned int l = strlen(line->data);
            memcpy(p, line->data, l);
            p += l;
        }
        *p = '\0';
    }

    avfilter_register_all();

    if (avfilter_graph_parse(graph, graph_string, NULL, NULL, NULL) < 0)
    {
        fprintf(stderr, "Impossible to parse the graph description\n");
        return 1;
    }

    if (avfilter_graph_check_validity(graph, NULL) ||
            avfilter_graph_config_formats(graph, NULL) ||
            avfilter_graph_config_links  (graph, NULL))
        return 1;

    print_digraph(outfile, graph);
    fflush(outfile);

    return 0;
}
