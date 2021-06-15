#! /usr/bin/env python3

"""
Select one analysis per word from hfst-optimized-lookup output
depending on the weight of the analysis.

A selector (HEAVIEST of LIGHTEST) given as commanline parameter
determines whether to output to analysis with most or least weight.
"""

from sys import stdin, argv, stderr

def print_analysis(weighted_analyses, selector):
    """
    Print eighted analysis in weighted analyses with either the
    highest or lowest weight depending on selector.
    """
    if weighted_analyses:
        weighted_analyses.sort()
        pos = 0
        if selector == 'HEAVIEST':
            pos = -1
        print('\t'.join(weighted_analyses[pos][1]))

if __name__=='__main__':
    if len(argv) != 2 or not argv[1] in ['HEAVIEST','LIGHTEST']:
        stderr.write('USAGE: %s [HEAVIEST|LIGHTEST]')
        exit(1)

    selector = argv[1]
    weighted_analyses = []
    for line in stdin:
        line = line.strip('\n')
    
        if line == '':
            print_analysis(weighted_analyses, selector)
            weighted_analyses = []            
        elif line.find('+?') != -1:
            print(line)
        else:
            wf, analysis, weight = line.split('\t')        
            weight = float(weight)
            weighted_analyses.append((weight, (wf, analysis)))

    print_analysis(weighted_analyses, selector)
