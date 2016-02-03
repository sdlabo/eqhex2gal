#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errors.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <float.h>
 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>
 
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>


int calc_average(char *filename, float *avgs)
{
  FILE* fp = fopen(filename, "r");
  char buf[1024];
  int16_t item[16];
  int t = 0;

  for(int i = 0; i < 15; i++){
    avgs[i] = 0;
  }

  uint64_t i = 0;
  while(char *p = fgets(buf, sizeof(buf), fp)){
    i++;

    if(i % 10000 == 0){
      printf("%6d行経過\n", i);
      fflush(stdout);
    }

    if(i >= 5){
      if(buf[0] == 'S' || buf[0] == '\r' || buf[0] == 'a' || buf[0] == 'D'){
        continue;
      }

      t++;

      sscanf(buf,
             "%04x\t%04x\t%04x\t"
             "%04x\t%04x\t%04x\t"
             "%04x\t%04x\t%04x\t"
             "%04x\t%04x\t%04x\t"
             "%04x\t%04x\t%04x\n",
             &item[0], &item[1], &item[2],
             &item[3], &item[4], &item[5],
             &item[6], &item[7], &item[8],
             &item[9], &item[10], &item[11],
             &item[12], &item[13], &item[14]);

      for(int j = 0; j < 15; j++){
        double alpha = 2.762937E-20;
        double beta = 5.922332E-16;
        double gamma = 6.582092E-6;
        double delta = 0.002276;

        double x = (double) item[j];
        double ad_val = alpha * x * x * x + beta * x * x + gamma * x + delta;
        double a = ad_val / 2.0 * 980.665;
        avgs[j] += a;
      }
    }else if(i == 1){
      if(strcmp(buf, "Sampling=1000\r\n") == 0){
        printf("1000 Hz sampling\n");
      }else if(strcmp(buf, "Sampling=100\r\n") == 0){
        printf("100 Hz sampling\n");
      }else{
        printf("ファイル形式が異なります。\r\n");
        printf("Enterで終了します。\n");
        gets(buf);
        exit(-1);
      }
    }
  }

  for(int i = 0; i < 15; i++){
    avgs[i] = avgs[i] / (double) t;
  }

  fclose(fp);
  return t;
}


int main(int argc, char** argv)
{
  if(argc != 2){
    char buf[1024];

    printf("ファイルをドラッグアンドドロップして下さい。\r\n\r\n");
    printf("コマンドラインからは\r\n");
    printf("%s [filename]\r\n", argv[0]);
    printf("で実行します。\r\n");
    exit(-1);
  }

  char csvfile[1024];
  sprintf(csvfile, "%s.csv", argv[1]);
  FILE* wp = fopen(csvfile, "w");
  if(wp == NULL){
    printf("ファイルが開けません: %s\n\n", csvfile);
    printf("Enterで終了します。\n");

    char buf[1024];
    gets(buf);
    exit(-1);
  }

  fprintf(wp,
          "time,"
          "x:70W7,y:70W7,z:70W7,"
          "x:70E1,y:70E1,z:70E1,"
          "x:70W1,y:70W1,z:70W1,"
          "x:70E7,y:70E7,z:70E7,"
          "x:65E1,y:65E1,z:65E1\n");

  float avgs[15];
  int num_line = calc_average(argv[1], avgs);

  printf("num_line = %d\n", num_line);

  int step;
  if(num_line == 600000){
    step = 10;
  }else{
    step = 1;
  }

  FILE* fp = fopen(argv[1], "r");
  char buf[1024];
  int16_t item[16];
  uint64_t i = 0;
  float t = 0.0;
  while(char *p = fgets(buf, sizeof(buf), fp)){
    i++;

    if(i % 10000 == 0){
      printf("%6d行経過\n", i);
      fflush(stdout);
    }

    if(i >= 5){
      if(buf[0] == 'S' || buf[0] == '\r' || buf[0] == 'a' || buf[0] == 'D'){
        continue;
      }

      if((i - 5) % step != 0){
        continue;
      }
      t++;

      sscanf(buf,
             "%04x\t%04x\t%04x\t"
             "%04x\t%04x\t%04x\t"
             "%04x\t%04x\t%04x\t"
             "%04x\t%04x\t%04x\t"
             "%04x\t%04x\t%04x\n",
             &item[0], &item[1], &item[2],
             &item[3], &item[4], &item[5],
             &item[6], &item[7], &item[8],
             &item[9], &item[10], &item[11],
             &item[12], &item[13], &item[14]);

      double val[15];
      for(int j = 0; j < 15; j++){
        double alpha = 2.762937E-20;
        double beta = 5.922332E-16;
        double gamma = 6.582092E-6;
        double delta = 0.002276;

        double x = (double) item[j];
        double ad_val = alpha * x * x * x + beta * x * x + gamma * x + delta;
        double a = ad_val / 2.0 * 980.665;
        val[j] = a - avgs[j];
      }

      fprintf(wp,
              "%0.03f,"
              "%e,%e,%e,"
              "%e,%e,%e,"
              "%e,%e,%e,"
              "%e,%e,%e,"
              "%e,%e,%e\n",
              t / 100.0,
              val[0], val[1], val[2],
              val[3], val[4], val[5],
              val[6], val[7], val[8],
              val[9], val[10], val[11],
              val[12], val[13], val[14]);
    }else if(i == 1){
      if(strcmp(buf, "Sampling=1000\r\n") == 0){
        printf("1000 Hz sampling\n");
      }else if(strcmp(buf, "Sampling=100\r\n") == 0){
        printf("100 Hz sampling\n");
      }else{
        printf("ファイル形式が異なります。\r\n");
        printf("Enterで終了します。\n");
        gets(buf);
        exit(-1);
      }
    }
  }

  fclose(fp);
  fclose(wp);
}
