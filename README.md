# xv6-riscv — Proyecto Final Sistemas Operativos

**Integrantes:**
- Alejandro Valencia Sandoval
- Maria Alejandra Pizarro

**Universidad:** Universidad del Valle  
**Curso:** Sistemas Operativos 2026

---

## Descripción general

Este proyecto modifica el sistema operativo académico xv6-riscv en dos componentes fundamentales:
1. El algoritmo de planificación de CPU (scheduler)
2. El sistema de gestión de memoria física

---

## Modificación 1: Scheduler MLFQ

**Archivos modificados:** `kernel/proc.h`, `kernel/proc.c`, `kernel/syscall.h`, `kernel/syscall.c`, `kernel/sysproc.c`, `user/user.h`, `user/usys.pl`

**Problema del scheduler original:**  
xv6 implementa Round Robin puro — recorre el arreglo de procesos en orden y le asigna un turno a cada uno sin distinguir entre procesos interactivos y procesos CPU-bound.

**Solución implementada — MLFQ (Multi-Level Feedback Queue):**  
Se implementaron 3 colas de prioridad con Round Robin en cada una:

| Cola | Prioridad | Quantum | Tipo de proceso |
|------|-----------|---------|-----------------|
| 0    | Alta      | 1 tick  | I/O-bound, interactivos |
| 1    | Media     | 4 ticks | Mixtos |
| 2    | Baja      | 8 ticks | CPU-bound |

**Mecanismo de aging:** procesos que esperan 20+ ticks suben una cola (previene starvation).

**Syscall agregada:** `getprio()` — retorna la cola de prioridad actual del proceso.

**Resultado demostrado:**
cat > README.md << 'EOF'
# xv6-riscv — Proyecto Final Sistemas Operativos

**Integrantes:**
- Alejandro Valencia Sandoval
- Maria Alejandra Pizarro

**Universidad:** Universidad del Valle  
**Curso:** Sistemas Operativos 2026

---

## Descripción general

Este proyecto modifica el sistema operativo académico xv6-riscv en dos componentes fundamentales:
1. El algoritmo de planificación de CPU (scheduler)
2. El sistema de gestión de memoria física

---

## Modificación 1: Scheduler MLFQ

**Archivos modificados:** `kernel/proc.h`, `kernel/proc.c`, `kernel/syscall.h`, `kernel/syscall.c`, `kernel/sysproc.c`, `user/user.h`, `user/usys.pl`

**Problema del scheduler original:**  
xv6 implementa Round Robin puro — recorre el arreglo de procesos en orden y le asigna un turno a cada uno sin distinguir entre procesos interactivos y procesos CPU-bound.

**Solución implementada — MLFQ (Multi-Level Feedback Queue):**  
Se implementaron 3 colas de prioridad con Round Robin en cada una:

| Cola | Prioridad | Quantum | Tipo de proceso |
|------|-----------|---------|-----------------|
| 0    | Alta      | 1 tick  | I/O-bound, interactivos |
| 1    | Media     | 4 ticks | Mixtos |
| 2    | Baja      | 8 ticks | CPU-bound |

**Mecanismo de aging:** procesos que esperan 20+ ticks suben una cola (previene starvation).

**Syscall agregada:** `getprio()` — retorna la cola de prioridad actual del proceso.

**Resultado demostrado:**
CPU-bound pid=4 started at queue 0
CPU-bound pid=4 moved to queue 1 (i=50000000)
CPU-bound pid=4 moved to queue 2 (i=100000000)
I/O-bound pid=5 started at queue 0
I/O-bound pid=5 done at queue 0 ← nunca fue demotado
---

## Modificación 2: Gestión de memoria

**Archivos modificados:** `kernel/kalloc.c`, `kernel/types.h`, `kernel/defs.h`, `kernel/syscall.h`, `kernel/syscall.c`, `kernel/sysproc.c`, `user/user.h`, `user/usys.pl`

**Problema del allocator original:**  
`kfree()` insertaba páginas liberadas al inicio de la free list sin importar su dirección física, generando fragmentación no detectable ni corregible.

**Solución implementada:**

- `kfree()` modificado para inserción ordenada por dirección física
- `kcoalesce()` integrado: merge de bloques físicamente adyacentes (requiere lista ordenada como prerequisito)
- `kgetmeminfo()`: expone métricas reales de memoria a user space

**Syscalls agregadas:**
- `meminfo()` — retorna páginas libres, usadas, total y bloques fragmentados
- `fragtest(n)` — ejecuta test de fragmentación con n páginas a nivel kernel
- `coalesce()` — ejecuta merge de bloques adyacentes desde user space

**Resultado demostrado:**
Before test: Frag blocks: 32494
During test: Frag blocks: 32478
After coalesce: Merges performed: 35
---

## Requisitos del entorno

- Ubuntu 26.04 LTS (recomendado via VirtualBox o WSL2)
- Toolchain RISC-V:

```bash
sudo apt-get install git build-essential gdb-multiarch \
  qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

---

## Compilación y ejecución

```bash
# Clonar el repositorio
git clone https://github.com/AlejandroVS-code/xv6-riscv-AVS-MAPS.git
cd xv6-riscv-AVS-MAPS

# Compilar y ejecutar xv6
make qemu

# Salir de xv6
Ctrl+A luego X
```

---

## Programas de prueba

### Test del scheduler MLFQ
```bash
$ mlfqtest
```
Demuestra democión progresiva de proceso CPU-bound (cola 0→1→2) y permanencia de proceso I/O-bound en cola 0.

### Test de gestión de memoria
```bash
$ memtest
```
Demuestra fragmentación de la free list y merges realizados por `coalesce()`.

---

## Estructura de ramas
riscv ← rama principal
feature/scheduler ← MLFQ scheduler
feature/memory-management ← gestión de memoria (mergeado via PR #1)
---

## Justificación teórica

**MLFQ:** algoritmo descrito en Silberschatz "Operating System Concepts" Cap. 5 y en el libro oficial de xv6 (book-riscv-rev3). Favorece procesos interactivos sin necesidad de conocer a priori el comportamiento de cada proceso.

**Lista ordenada + coalescencia:** prerequisito clásico para cualquier allocator con merge de bloques. Documentado en Tanenbaum "Modern Operating Systems" Cap. 3. La ordenación por dirección física garantiza que bloques adyacentes sean detectables en O(n).
