# C kodlarını derlemek için standart GCC (GNU Compiler Collection) imajını kullanıyoruz
FROM gcc:latest

# Konteyner içindeki çalışma dizinimiz
WORKDIR /app

# Projemizdeki src (kaynak) klasörünü Docker'ın içine kopyalayacağız
# İleride kodlar yazıldığında burayı aktif edeceğiz
# COPY ./src /app/src

# Varsayılan başlatma komutu (Şimdilik sadece çalışıp kapanmaması için bir log atıyoruz)
# İleride şu olacak: RUN gcc -o dom_tree src/core/*.c
CMD ["echo", "C Derleyicisi Hazır. Kodlar eklendiğinde derleme yapılacaktır."]
