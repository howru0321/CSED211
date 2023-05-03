#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "cachelab.h"

char* trace_name;
int o;

int s, E, b;//s는 set index의 bit 수, E는 한 set당 line의 개수, b는 block offset의 bit수 이다.

int hit_count, miss_count, eviction_count = 0;//printSummary(hit_count, miss_count, eviction_count) 함수에 들어갈 매개변수들이다.

int time_temp = 0;
int c_time = 0;



/*//한 line에 들어있는 정보를 담은 struct. 이 문제에서는 cache의 성능만을 측정하므로 caching 과정 중 block offset에서 정보를 읽는 상황은 고려하지 않으므로 cache block는 굳이 넣지 않아도 된다.*/
typedef struct {
   int valid;
   int tag;
   int time;
}line;

line** cache;//cache 정보를 담은 배열. set개수만큼 동적할당하고, 각 set 마다 E만큼 동적할당할 것이다.

/*caching을 하는 함수*/
void caching_f(unsigned long long address)
{
   int tag = address >> (s + b);//tag bit 추출
   int set_index = (address >> b) - (tag << s);//set_index bit 추출

   c_time++;
   time_temp = c_time;

   bool hit_cache = false;//보고자 하는 set 안에 있는 모든 line을 검사했을때 hit를 하지 못하면(miss가 나면) false를 가진다.
   bool eviction_cache = true;//보고자 하는 set에 이미 있는 line을 다른 line으로 교체될때 true를 가진다. 모든 line이 다 차 있으면(모든 line의 valid값이 1이면) 필연적으로 ture를 가진다.

   int pos = 0;//이 반복문을 다 돌았을때 최종적으로 있었던 위치. miss가 났을때 line 교체가 일어나지 않을 경우 정보를 넣을 line의 위치를 담고, hit가 났을때 hit 위치정보를 가지고 있는 linked-list 정보를 찾아 삭제하기 위해 hit의 위치정보를 담아야 하는 변수이다.

   int i;
   for (i = 0; i < E; i++)//보고자 하는 set 안에 있는 모든 line을 검사하는 반복문
   {
      if (cache[set_index][i].time < time_temp)
      {
         time_temp = cache[set_index][i].time;
         pos = i;//hit가 될 경우에는 위치정보를 알 필요가 없고, valid가 0일경우 그 위치의 line에 정보를 넣으면 된다. 이 두 경우를 제외하면 모든 loop를 돌 수 있으므로 이 위치는 최소최근사용라인이다.
      }

      if (cache[set_index][i].valid != 0)//vaild가 1일때이다. 이 경우 block에 원하는 정보가 담겨 있는지(tag가 일치하는지) line을 검사해 봐야한다.(실제로는 block는 없지만...)
      {
         if (cache[set_index][i].tag == tag)//block에 원하는 정보가 있는 경우(tag가 일치하는 경우)이다.
         {
            hit_cache = true;
            break;//밑에서 hit_count++말고는 더 이상 진행해야하는 일이 없으므로(line을 추가하거나 삭제하지 않는다.) break로 반복문을 종료한다.
         }
      }
      else//vaild가 0일때다. 이 코드에서 set배열(cache[i][]배열)에는 line이 순서대로 정보가 적히므로, 지금 검사하는 line에서 valid가 0이 나왔다는 것은 앞에서 원하는 정보가 담긴 block가 있는 line을 찾지 못한 것이다. 즉 miss가 난 것이다.
      {
         pos = i;//i 보다 큰 경우 모두 valid가 0일것이므로 뒤에는 볼 필요도 없고, 이 위치를 기억해서 여기에 정보를 넣으면 된다.
         eviction_cache = false;//이 위치에 있는 비어있는 line에 정보를 넣으므로, 교체할 line이 없다.
         break;//뒤에 부분은 더이상 검사할 필요가 없다. break를 이용해 반복문을 종료한다.
      }
   }

   if (hit_cache)
   {
      hit_count++;
      cache[set_index][i].time = c_time;
   }
   else//miss를 한 경우이다.
   {
      miss_count++;

      if (eviction_cache)//교체할 line이 있는 경우이다.
      {
         eviction_count++;
      }
      else//교체할 line이 없는 경우이다.
      {
         /*정보가 없는 line에 정보를 넣는다*/
         cache[set_index][pos].valid = 1;
      }
      cache[set_index][pos].tag = tag;//교체된 line에 있는 tag 값을 최근에 추가한 정보의 tag값으로 교체한다.
      cache[set_index][pos].time = c_time;
   }
}

int main(int argc, char** argv)
{
   FILE* file;
   char operation;
   unsigned long long address;
   int size;

   while ((o = getopt(argc, argv, "s:E:b:t: ")) != -1)
   {
      switch (o)
      {
      case 'h':
         printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
         return 0;
      case 'v':
         break;
      case 's':
         s = atoi(optarg);
         break;
      case 'E':
         E = atoi(optarg);
         break;
      case 'b':
         b = atoi(optarg);
         break;
      case 't':
         trace_name = optarg;
         break;
      default:
         exit(1);
      }
   }

   file = fopen(trace_name, "r");
   if (file == NULL)
      return 0;

   int S = 1 << s;
   cache = (line**)malloc(sizeof(line*) * S);
   int i, j;
   for (i = 0; i < S; i++)
   {
      cache[i] = (line*)malloc(sizeof(line) * E);

      for (j = 0; j < E; j++)
      {
         cache[i][j].tag = 0;
         cache[i][j].valid = 0;
         cache[i][j].time = 0;
      }
   }

   if (file != NULL)
   {
      while (fscanf(file, " %c %llx %d", &operation, &address, &size) != EOF)
      {
         if (operation == 'I') {}
         else if (operation == 'L')
            caching_f(address);
         else if (operation == 'S')
            caching_f(address);
         else if (operation == 'M')
         {
            caching_f(address);
            caching_f(address);
         }
         else {}
      }
   }
   printSummary(hit_count, miss_count, eviction_count);

   fclose(file);

   return 0;
}
