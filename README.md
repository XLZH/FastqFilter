# FastqFilter
The Three sourcecode stand for different version!

Filter the low quality fastq reads before processing it!
The FastqFilter.3.c support the input of <\*.gz>!

# compile
    (1) version <= 2
        gcc -std=c99 -o FastqFilter FastqFilter.<version>.c
    (2) version > 2
        gcc -std=c99 -o FastqFilter FastqFilter.3.c -lz

# Usage
    ./FastqFilter <InR1.fq/gz> <InR2.fq/gz> <minQ> <maxpercent minQ> <outR1.fq> <outR2.fq>

    minQ : minmum quality for fastq base
    maxpercent minQ: the maxmum percent for minQ occoured in fastq base!
