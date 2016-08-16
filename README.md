# FastqFilter
Filter the low quality fastq reads before processing it!

(1) compile : gcc -std=c99 -o FastqFilte FastqFilter.c

(2) Usage: FastqFilter <InR1.fq> <InR2.fq> <minQ> <maxpercent minQ> <outR1.fq> <outR2.fq>

    minQ : minmum quality for fastq base
    maxpercent minQ: the maxmum percent for minQ occoured in fastq base!
