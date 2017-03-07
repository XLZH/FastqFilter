/* Purpose: To filter out the low quality reads
*  Require: Paired-end fastqfile
*  
*  NOTE: The default of MAX_READLEN is 200(mean the maxlength of one row)
*  So you'd better modify it if you have a reads length larger than 200!
*
*  -------------------- Args bellow ------------------------------------
*
*  [ inR1.fq ] (argv[1]) : input fastqR1   
*  [ inR2.fq ] (argv[2]) : input fastqR2
*  [ minQ ]    (argv[3]) : minimum base quality(define as low quality)
*  [ maxpercent minQ ] (argv[4]): maximum percent of the low quality base!
*  [ outR1.fq] (argv[5]) : the file name of output fastqR1!    
*  [ outR2.fq] (argv[6]) : the file name of output fastqR2!
*
*  Wirte Time: 2016-4-8
*  Version: 1
*  Author :XLZH
*/


#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_READLEN 200
#define MAX_BUFROW 100000

char TemBufR1[4][MAX_READLEN] = {'\0'};
char TemBufR2[4][MAX_READLEN] = {'\0'};
char WriteBufR1[MAX_BUFROW][MAX_READLEN] = {'\0'};
char WriteBufR2[MAX_BUFROW][MAX_READLEN] = {'\0'};


void Usage(char *argv)
{
    printf("Usage:\n");
    printf("\t%s <inR1.fq> <inR2.fq> <minQ> <maxpercent minQ> <outR1.fq> <outR2.fq>\n", argv);
    printf("Example:\n");
    printf("\t%s XXXR1.fq XXXR2.fq 5 10 XXXFilterR1.fq XXXFilterR2.fq\n", argv);
}


void Addreads(int WriteBufreadscount, char (*TemBuf)[MAX_READLEN], char (*WriteBuf)[MAX_READLEN])
{
    int i = 0;
    int j = WriteBufreadscount - 4;

    for(i,j; i < 4; ++i,++j)
    {
        strcpy (*(WriteBuf + j), *(TemBuf + i));
    }
}


bool Filter(char *Tem, int minQ, int maxpercent)
{
    int lowcount = 0;
    register int ASCII = minQ + 33;
    int maxTolerance = (int)(maxpercent * 0.01 * strlen(Tem));
    
    for(; *Tem != '\n'; ++Tem)
    {
        if(*Tem < ASCII) ++lowcount;
    }
    
    if(lowcount <= maxTolerance) return true;
    else return false;
}


void Write(int Bufreadscount, char (*WriteBuf)[MAX_READLEN], char *outfq)
{
    FILE *poutfile = NULL;
    
    if (poutfile = fopen(outfq,"a"))
    {
        for (int i=0; i < Bufreadscount; ++i)
            fputs(*(WriteBuf + i), poutfile);
    }

    fclose(poutfile);
    poutfile = NULL;
}


int main(int argc, char **argv)
{
    if(argc != 7)
    {
        Usage(argv[0]);
        exit(1);
    }

    FILE *pR1 = NULL;
    FILE *pR2 = NULL;
    bool fgetstatus = true;
    long int WriteBufreadscount = 0L;
    long int Allreadscount = 0L;    

    if(!((pR1 = fopen(argv[1],"r")) && (pR2 = fopen(argv[2],"r"))))
    {
        // argv[1]: input fastqR1   argv[2]: input fastqR2

        printf("Failed to open %s or %s\n", argv[1], argv[2]);
        exit(2);
    }
    setvbuf(pR1, NULL, _IOFBF, BUFSIZ);
    setvbuf(pR2, NULL, _IOFBF, BUFSIZ);

    while(fgetstatus)
    {
        for(int i=0; i < 4; ++i)
        {
            if(fgets(TemBufR1[i], MAX_READLEN, pR1) && fgets(TemBufR2[i], MAX_READLEN, pR2))

                fgetstatus = true;
            else
            {
                fgetstatus = false;
                goto Over;
            }
        }

        bool R1keepornot = Filter(TemBufR1[3], atoi(argv[3]), atoi(argv[4]));
        bool R2keepornot = Filter(TemBufR2[3], atoi(argv[3]), atoi(argv[4]));

        if(R2keepornot && R1keepornot)
        {   
            // argv[3]: minimum base quality(define as low quality)
            // argv[4]: maximum percent of the low quality base!
            // argv[5]: outfile1     argv[6]: outfile2

            WriteBufreadscount += 4;
            Addreads(WriteBufreadscount, TemBufR1, WriteBufR1);
            Addreads(WriteBufreadscount, TemBufR2, WriteBufR2);
            
            if(WriteBufreadscount >= MAX_BUFROW)
            {
                Allreadscount += WriteBufreadscount;
                printf("\r[*]   %d reads has been writen out!", Allreadscount);
		fflush(stdout);
                Write(WriteBufreadscount, WriteBufR1, argv[5]);
                Write(WriteBufreadscount, WriteBufR2, argv[6]);
                WriteBufreadscount = 0L;
            }
         }

         Over:
             if(!fgetstatus)
             {
                 Allreadscount += WriteBufreadscount;
                 printf("\r[*]   %d reads has been writen out!\n", Allreadscount);
                 Write(WriteBufreadscount, WriteBufR1, argv[5]);
                 Write(WriteBufreadscount, WriteBufR2, argv[6]);
             }
    }
    
    fclose(pR1);
    fclose(pR2);
    pR1 = NULL;
    pR2 = NULL;

}
