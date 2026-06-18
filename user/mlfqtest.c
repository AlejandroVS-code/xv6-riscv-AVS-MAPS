#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  printf("=== MLFQ Scheduler Metrics Test ===\n\n");
  printf("Parent pid=%d starting at queue %d\n\n", getpid(), getprio());

  // --- CPU-bound process ---
  int pid_cpu = fork();
  if(pid_cpu == 0){
    struct procmetrics pm;
    printf("CPU-bound  pid=%d started at queue %d\n", getpid(), getprio());
    int last_prio = -1;
    volatile long i = 0;
    for(i = 0; i < 500000000L; i++){
      if(i % 50000000L == 0){
        int prio = getprio();
        if(prio != last_prio){
          printf("CPU-bound  pid=%d moved to queue %d (i=%ld)\n", getpid(), prio, i);
          last_prio = prio;
        }
      }
    }
    getmetrics(&pm);
    printf("CPU-bound  pid=%d done at queue %d\n", getpid(), getprio());
    printf("  arrival_tick   : %ld\n", pm.arrival_tick);
    printf("  first_run_tick : %ld\n", pm.first_run_tick);
    printf("  response_time  : %ld ticks\n", pm.response_time);
    printf("  total_wait     : %ld ticks\n", pm.total_wait);
    exit(0);
  }

  // --- I/O-bound process ---
  int pid_io = fork();
  if(pid_io == 0){
    struct procmetrics pm;
    printf("I/O-bound  pid=%d started at queue %d\n", getpid(), getprio());
    for(int j = 0; j < 5; j++){
      pause(5);
      printf("I/O-bound  pid=%d woke up iter %d at queue %d\n", getpid(), j, getprio());
    }
    getmetrics(&pm);
    printf("I/O-bound  pid=%d done at queue %d\n", getpid(), getprio());
    printf("  arrival_tick   : %ld\n", pm.arrival_tick);
    printf("  first_run_tick : %ld\n", pm.first_run_tick);
    printf("  response_time  : %ld ticks\n", pm.response_time);
    printf("  total_wait     : %ld ticks\n", pm.total_wait);
    exit(0);
  }

  (void)pid_cpu;
  (void)pid_io;
  wait(0);
  wait(0);
  printf("\n=== MLFQ Metrics Test Complete ===\n");
  exit(0);
}
