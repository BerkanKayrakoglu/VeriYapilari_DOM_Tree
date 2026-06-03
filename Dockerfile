# --- Derleme Aşaması (Build Stage) ---
FROM alpine:latest AS builder

# Gerekli derleme araçlarını (gcc, musl-dev) kuruyoruz
RUN apk add --no-cache gcc musl-dev

WORKDIR /build

# Kaynak kodları kopyalıyoruz
COPY src /build/src

# C projesini derliyoruz (main.c hariç, çünkü hem main.c hem server.c içerisinde main() fonksiyonu bulunuyor)
RUN gcc -o server \
    src/core/server.c \
    src/core/parser.c \
    src/core/n_ary_tree.c \
    src/core/hash_table.c \
    src/core/stack.c \
    src/core/queue.c

# --- Çalıştırma Aşaması (Runtime Stage) ---
FROM alpine:latest

WORKDIR /app

# Derlenen çalıştırılabilir sunucu dosyasını kopyalıyoruz
COPY --from=builder /build/server /app/server

# HTTP sunucu portunu dışarı açıyoruz
EXPOSE 8080

# Sunucuyu başlatıyoruz
CMD ["./server"]
