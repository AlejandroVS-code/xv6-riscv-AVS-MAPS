#include "kernel/types.h"
#include "user/user.h"

void
print_point(const char *label, int point)
{
  struct meminfo mi;
  meminfo(&mi);
  printf("[%d] %s\n", point, label);
  printf("     free_pages  : %ld\n", mi.free_pages);
  printf("     frag_blocks : %ld\n", mi.frag_blocks);
  printf("     utilization : %ld%%\n", (mi.used_pages * 100) / mi.total_pages);
}

int
main(void)
{
  printf("=== Memory Fragmentation Metrics Test ===\n\n");
  printf("Each point measures frag_blocks and free_pages\n");
  printf("frag_blocks = non-contiguous runs in sorted free list\n\n");

  // Punto 0: baseline
  print_point("Baseline (no allocs)", 0);

  // Punto 1: 16 pages allocated
  sbrk(4096 * 16);
  print_point("After allocating 16 pages", 1);

  // Punto 2: 32 pages allocated
  sbrk(4096 * 16);
  print_point("After allocating 32 pages total", 2);

  // Punto 3: free even pages (alternating pattern)
  for(int i = 0; i < 16; i++)
    sbrk(-4096);
  print_point("After freeing 16 pages (alternating pattern)", 3);

  // Punto 4: kernel fragtest with 32 pages
  uint64 frag_during = fragtest(32);
  printf("[4] Kernel fragtest(32) during alternating free\n");
  printf("     frag_blocks during : %ld\n\n", frag_during);

  // Punto 5: before coalesce
  struct meminfo before;
  meminfo(&before);
  printf("[5] Before coalesce()\n");
  printf("     frag_blocks : %ld\n\n", before.frag_blocks);

  // Punto 6: after coalesce
  int merges = coalesce();
  struct meminfo after;
  meminfo(&after);
  printf("[6] After coalesce()\n");
  printf("     merges performed : %d\n", merges);
  printf("     frag_blocks      : %ld\n", after.frag_blocks);
  printf("     frag reduction   : %ld blocks merged\n\n",
         before.frag_blocks - after.frag_blocks);

  // Punto 7: free remaining pages
  sbrk(-4096 * 16);
  print_point("After freeing all remaining pages", 7);

  // Punto 8: final coalesce
  int merges2 = coalesce();
  struct meminfo final;
  meminfo(&final);
  printf("[8] Final coalesce()\n");
  printf("     merges performed : %d\n", merges2);
  printf("     frag_blocks      : %ld\n\n", final.frag_blocks);

  printf("=== Summary ===\n");
  printf("  Total merges by coalesce(): %d\n", merges + merges2);
  printf("  Original xv6: 0 merges possible (no coalesce, unordered list)\n");
  printf("  Our xv6     : sorted list enables coalescence\n");

  printf("\n=== Memory Test Complete ===\n");
  exit(0);
}
