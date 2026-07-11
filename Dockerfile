FROM ubuntu:24.04

# Evitar prompts interactivos durante la instalación
ENV DEBIAN_FRONTEND=noninteractive

# Instalar toolchain RISC-V y dependencias
RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    gdb-multiarch \
    qemu-system-misc \
    gcc-riscv64-linux-gnu \
    binutils-riscv64-linux-gnu \
    && rm -rf /var/lib/apt/lists/*

# Directorio de trabajo
WORKDIR /xv6

# Clonar el repositorio del proyecto
RUN git clone https://github.com/AlejandroVS-code/xv6-riscv-AVS-MAPS.git .

# Compilar xv6
RUN make

# Puerto no aplica — xv6 corre en terminal interactiva
# Para correr: docker run -it <imagen>
CMD ["make", "qemu"]
