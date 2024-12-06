#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>                               			
void *print_hello (void *arg)
{
   int i = (int)arg;

   printf("it's me, thread %d!\n", i);

   *****补全***** ;                                     	/*  线程退出               */

   return (NULL);
}
int  main (int  argc, char  *argv[])
{
   int 		 ret;
   int 		 i;
   pthread_t	 tid[5];

   for (i = 0; i < 5; i++) {                          		/*  创建 5 个线程          */
      printf("in main: creating thread %d\n", i);

      *****补全***** ;//线程创建并返回状态ret
      if (ret != 0) {
         printf("create thread failed\n");
         exit(-1);
      }
   }
   printf("in main: exit!\n");
   *****补全***** ;                          			/*  创建线程后，主线程退出 */

   return (0);
}
