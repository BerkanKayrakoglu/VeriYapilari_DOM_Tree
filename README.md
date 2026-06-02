# Veri Yapıları ile HTML'den DOM Ağacı Oluşturma

Bu proje, saf metin halindeki (raw string) bir HTML dokümanını C dilinde ayrıştırarak (parsing) bellekte hiyerarşik bir **DOM (Document Object Model)** ağacına dönüştüren ve üzerinde çeşitli algoritmalar çalıştıran bir sistemdir.

## 👥 Proje Ekibi (Ara Rapor: 30.04.2026)

Bu depo, projenin geliştirme ve takım içi koordinasyon merkezi olarak kullanılmaktadır. Takım üyeleri ve görev dağılımları aşağıda belirtilmiştir:

*   **@bkayr** : Sistem mimarisi, Docker/DevOps süreçleri ve C - Frontend haberleşme altyapısı.
*   **@hamzayavas** : C dilinde Temel Veri Yapılarının (N-ary Tree, Stack, Queue, Hash Table) sıfırdan kodlanması.
*   **@MstfOzturk16** : Frontend (Kullanıcı Arayüzü) geliştirilmesi ve ayrıştırma (parsing) algoritmalarının entegrasyonu.

### 📝 Ara Rapor: Bugüne Kadar Alınan Kararlar ve Gelişmeler
Proje başlangıcından itibaren takım olarak aşağıdaki kararlar alınmış ve altyapı çalışmaları tamamlanmıştır:
1.  **Dil ve Teknoloji Seçimi:** Performans ve bellek yönetimi (Data Structures) amacıyla çekirdek motorun **C dili** ile yazılmasına karar verilmiştir. Arayüz için ise HTML/CSS/JS kullanılacaktır.
2.  **Versiyon Kontrol Stratejisi:** Geliştirme süreçleri doğrudan `main` branch'ine atılmayacak, her üye kendi branch'ini (`feat/data-structures`, `feat/frontend` vb.) oluşturup **Pull Request (PR)** mekanizması ile kod incelemesinden sonra birleştirme (merge) yapacaktır.
3.  **Mikroservis/Asenkron Yapı:** C kodunun çalışacağı ana sistem ile Yapay Zeka (AI) test/veri üretme servisinin birbirinden bağımsız (asenkron) çalışması planlanmıştır. Docker entegrasyonu için ilk iskeletler oluşturulmuştur.

---

## 🏗️ Proje Mimarisi ve Veri Yapıları

Projede standart kütüphaneler **kullanılmadan** aşağıdaki veri yapıları sıfırdan implemente edilmektedir:

*   **N-ary Tree:** DOM hiyerarşisini (parent-child ilişkileri) bellekte tutmak için.
*   **Stack (Yığıt):** HTML metnini okurken açılış ve kapanış etiketlerinin (tag) eşleşmelerini kontrol etmek için.
*   **Queue (Kuyruk):** DOM ağacı üzerinde Genişlik Öncelikli Arama (BFS) yapabilmek için.
*   **Hash Table (Karma Tablo):** Düğümlere atanan `id` değerlerini O(1) karmaşıklığında hızlıca bulabilmek için.

## 🚀 Kurulum ve Çalıştırma

Projeyi yerel makinenizde çalıştırmak için iki yöntem bulunmaktadır:

### 1. Docker Compose ile Çalıştırma (Tavsiye Edilen)

Tüm servisleri (C Backend, Nginx ile Frontend ve Python AI Servisi) tek bir komutla ayağa kaldırmak için projenin ana dizininde terminalden şu komutu çalıştırın:

```bash
docker-compose up --build -d
```

Bu komut çalıştığında:
- **C Backend (dom_tree_core):** `http://localhost:8080` portunda çalışmaya başlar ve gelen HTML ayrıştırma isteklerini karşılar.
- **Frontend Arayüzü (dom_tree_ui):** `http://localhost:3000` portundan web arayüzünü sunar.
- **AI Servisi (dom_tree_ai):** `http://localhost:5001` portunda sağlık kontrolü ve veri entegrasyonu için hazır durumda bekler.

Servisleri durdurmak için:
```bash
docker-compose down
```

### 2. Manuel Derleme ve Çalıştırma (Docker Olmadan)

Eğer sisteminizde Docker kurulu değilse, C Backend sunucusunu yerel derleyiciniz (GCC) ile şu şekilde çalıştırabilirsiniz:

**Windows (MinGW):**
```bash
gcc -o server src/core/server.c src/core/parser.c src/core/n_ary_tree.c src/core/hash_table.c src/core/stack.c src/core/queue.c -lws2_32
./server.exe
```

**Linux / macOS:**
```bash
gcc -o server src/core/server.c src/core/parser.c src/core/n_ary_tree.c src/core/hash_table.c src/core/stack.c src/core/queue.c
./server
```

Ardından `frontend/index.html` dosyasını tarayıcınızda açarak arayüzü kullanabilirsiniz.