#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>                               			

void *print_hello (void *arg)
{
   int i = (int)arg;
   // sleep(1);

   printf("it's me, thread %d!\n", i);
   

   pthread_exit(NULL);                                     	/*  线程退出               */

   return (NULL);  
}

int main (int argc, char *argv[])
{
   int ret;
   int i;
   pthread_t tid[5];

   for (i = 0; i < 5; i++) {                          		/*  创建 5 个线程          */
      printf("in main: creating thread %d\n", i);

      ret = pthread_create(&tid[i], NULL, print_hello, (void *)i);  //线程创建并返回状态ret
      if (ret != 0) {
         printf("create thread failed\n");
         exit(-1);
      }
   }
   pthread_join(tid[0], NULL); //////////////////////////////////////////////////////////
   
   printf("in main: exit!\n");
   pthread_exit(NULL);                          			/*  创建线程后，主线程退出 */

   return (0);
}
