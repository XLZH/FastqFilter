/* Purpose: To filter out the low quality reads
*  Require: Paired-end fastqfile
*  
*  NOTE: The default of MAX_READLEN is 200(mean the maxlength of one row)
*  So you'd better modify it if you have a reads length larger than 200!
*  NEW: Support the (fastq.gz) file now!
*
*  -------------------- Args bellow ------------------------------------
*
*  [ inR1.fq ] (argv[1]) : input fastqR1(.gz)   
*  [ inR2.fq ] (argv[2]) : input fastqR2(.gz)
*  [ minQ ]    (argv[3]) : minimum base quality(define as low quality)
*  [ maxpercent minQ ] (argv[4]): maximum percent of the low quality base!
*  [ maxpercent N ] (argv[5]): maximum percent of N base allowed!
*  [ outR1.fq] (argv[6]) : the file name of output fastqR1!    
*  [ outR2.fq] (argv[7]) : the file name of output fastqR2!
*
*  Wirte Time: 2017-3-06
*  Version : 3
*  Author :XLZH
*/


#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <stdbool.h>

#define MAX_READLEN 200
#define MAX_BUFROW 100000
#define N 35 // quality of N  ( # ascii = 35)

char TemBufR1[4][MAX_READLEN] = {'\0'};
char TemBufR2[4][MAX_READLEN] = {'\0'};
char WriteBufR1[MAX_BUFROW][MAX_READLEN] = {'\0'};
char WriteBufR2[MAX_BUFROW][MAX_READLEN] = {'\0'};


void Usage(char *argv)
{
    printf("Version: 2\n");
    printf("Usage:\n");
    printf("\t%s <inR1.fq/inR1.fq.gz> <inR2.fq/inR2.fq.gz> <minQ> <maxpercent minQ> <maxpercent N> <outR1.fq> <outR2.fq>\n", argv);
    printf("Example:\n");
    printf("\t%s XXXR1.fq.gz XXXR2.fq.gz 5 10 10 XXXFilterR1.fq XXXFilterR2.fq\n", argv);
}


void Addreads (int WriteBufreadscount, char (*TemBuf)[MAX_READLEN], char (*WriteBuf)[MAX_READLEN])
{
    int i = 0;
    int j = WriteBufreadscount - 4;

    for(i,j; i < 4; ++i,++j) {
        strcpy (*(WriteBuf + j), *(TemBuf + i));
    }
}


bool Filter (char *Tem, int minQ, int low_max, int n_max)
{
    int lowcount, Ncount, length;

    lowcount = Ncount = 0;
    length = strlen(Tem);
    register int ASCII = minQ + 33;
    int low_tolerance = (int)(low_max * 0.01 * length);
    int n_tolerance = (int)(n_max * 0.01 * length);
    
    for(; *Tem != '\n'; ++Tem) {
        if ( *Tem < ASCII ) ++lowcount;
        if ( *Tem == N ) ++Ncount;
    }
    
    if (lowcount <= low_tolerance && Ncount <= n_tolerance) return true;
    else return false;
}


void Write(int Bufreadscount, char (*WriteBuf)[MAX_READLEN], char *outfq)
{
    FILE *poutfile = NULL;
    
    if (poutfile = fopen(outfq,"a")) {
        for (int i=0; i < Bufreadscount; ++i)
            fputs(*(WriteBuf + i), poutfile);
    }
    fclose(poutfile); poutfile = NULL;
}


int main(int argc, char **argv)
{
    if (argc != 8) {
        Usage(argv[0]); exit(1);
    }

    gzFile pR1, pR2;
    bool fgetstatus = true;
    long int WriteBufreadscount = 0L, Allreadscount = 0L;

    if(!((pR1 = gzopen(argv[1],"r")) && (pR2 = gzopen(argv[2],"r"))))
    {
        // argv[1]: input fastqR1   argv[2]: input fastqR2
        printf("Failed to open %s or %s\n", argv[1], argv[2]);
        exit(2);
    }
    gzbuffer(pR1, 524288); gzbuffer(pR2, 524288);

    while (fgetstatus) {
        for(int i=0; i < 4; ++i) {
            if (gzgets(pR1, TemBufR1[i], MAX_READLEN) && \
                            gzgets(pR2, TemBufR2[i], MAX_READLEN)) {
                fgetstatus = true;
            }
            else {
                fgetstatus = false; goto Over;
            }
        }

        bool R1keepornot = Filter(TemBufR1[3], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        bool R2keepornot = Filter(TemBufR2[3], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));

        if(R2keepornot && R1keepornot) 
        {   
            WriteBufreadscount += 4;
            Addreads(WriteBufreadscount, TemBufR1, WriteBufR1);
            Addreads(WriteBufreadscount, TemBufR2, WriteBufR2);
            
            if(WriteBufreadscount >= MAX_BUFROW)
            {
                Allreadscount += WriteBufreadscount;
                printf("\r[*]   %ld reads has been writen out!", Allreadscount);
		        fflush(stdout);
                Write(WriteBufreadscount, WriteBufR1, argv[6]);
                Write(WriteBufreadscount, WriteBufR2, argv[7]);
                WriteBufreadscount = 0L;
            }
         }

         Over:
             if(!fgetstatus) {
                 Allreadscount += WriteBufreadscount;
                 printf("\r[*]   %ld reads has been writen out!\n", Allreadscount);
                 Write(WriteBufreadscount, WriteBufR1, argv[6]);
                 Write(WriteBufreadscount, WriteBufR2, argv[7]);
             }
    }
    
    gzclose(pR1); gzclose(pR2);
    pR1 = NULL; pR2 = NULL;
}
