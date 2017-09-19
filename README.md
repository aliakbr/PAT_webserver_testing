# PAT_webserver_testing

## Tentang
Project ini dibuat untuk memenuhi tugas perkuliahan IF4031 Pengantar Aplikasi Terdistribusi. Project ini dibangun dalam lingkungan sistem Ubuntu 16.04 LTS

## Instalasi & Kebutuhan Kakas
Untuk menjalankan project ini semua kebutuhan di bawah harus terinstall pada sistem anda :

- Apache Sever

Dapat diinstall melalui XAMPP atau diinstall secara terpisah melalui modul apt. Pada tugas ini Apache server diinstall melalui [XAMPP](https://www.apachefriends.org/download.html).

- NGINX

Diinstall melalui apt `sudo apt-get install nginx`

- Apache Benchmark

Diinstall melalui apt `sudo apt-get install apache2-utils`

- GCC

Jika menggunakan lingkungan sistem yang sama maka GCC sudah otomatis terinstall. Untuk informasi installasi pada lingungan yang berbeda dapat melihat [tautan berikut](https://gcc.gnu.org/install/download.html)

- libev-dev module

Diinstall melalui apt `sudo apt-get install libev-dev`

## Membangun Project dan Mengonfigurasi Kakas

### Apache Server
- Untuk membuka GUI dari XAMPP dapat dilakukan dengan `sudo /opt/lampp/lampp start`
- Pada GUI, pilih tab `Manage Servers`, pilih `Apache Web Server` dan klik `Start` untuk memulai service. Untuk melakukan konfigurasi terhadap webserver klik `Configure`
- Untuk konfigurasi melalui CLI dapat dilakukan dengan mengedit file `/etc/apache2/ports.conf` melalui text editor kesukaan anda. Ubah bagian `Listen <Port>` menggunakan port yang kosong (Jika port default sudah digunakan).
- Letakkan halaman html pada folder `html/` di project ini ke `root` directory modul apache anda. Untuk XAMPP letakkan pada `/opt/lampp/htdocs/`. Jika menggunakan apache2 terpisah letakkan pada `/var/www/html/`
- Untuk memulai service melalui CLI dapat dilaukan dengan `sudo service apache2 start`

### NGINX
- Letakkan halaman html pada folder `/var/www/html/`
- Untuk setting port dapat dilakukan dengan mengedit file `/etc/nginx/sites-enabled/default` pada bagian `listen <port>`
- Jalankan servie dengan `sudo service nginx start`

### libev program
- Jalankan makefile yang terdapat pada direktori ini untuk mengompilasi program dengan menggunakan command `make`
- Jalankan `./server` untuk menjalankan server. Server dapat diakses pada `http://127.0.0.1:3033/500b.html`

## Melakukan Pengujian

Pengujian dilakukan dengan Apache Benchmark. Untuk melakukan pengujian jalankan command berikut :

`ab -n 10000 -c 10000 http://<IP Address>:<Port>/<Path>`

Contoh command yang digunakan dalam pengujian project ini adalah sebagai berikut :

`ab -n 10000 -c 10000 http://127.0.0.1:80/500b.html`

Untuk mengobservasi usage yang digunakan oleh server kami melakukan monitoring menggunakan `htop`

Skema pengujian :
- Nyalakan server
- Jalankan load test dengan Apache Benchmark
- Perhatikan memory usage pada `htop` saat menjalankan load test, dari tiap proses/worker yang berjalan untuk server hitung total memory usagenya

## Hasil Pengujian

### Apache Server
Pada tugas ini Apache Server dijalankan pada port 80 localhost.

- Halaman 500 bytes

`ab -n 10000 -c 10000 http://127.0.0.1:80/500b.html`

![Screenshot](/img/Apache%20500b.png)

- Halaman 20KB

`ab -n 10000 -c 10000 http://127.0.0.1:80/20KB.html`

![Screenshot](/img/Apache%2020KB.png)

### NGINX

Pada tugas ini NGINX dijalankan pada port 80 localhost.

- Halaman 500 bytes

`ab -n 10000 -c 10000 http://127.0.0.1:80/500b.html`

![Screenshot](/img/NGINX%20500b.png)

- Halaman 20KB

`ab -n 10000 -c 10000 http://127.0.0.1:80/20KB.html`

![Screenshot](/img/NGINX%2020KB.png)

### libev

Pada tugas ini program libev dijalankan pada port 3033 localhost.

- Halaman 500 bytes

`ab -n 10000 -c 10000 http://127.0.0.1:3033/500b.html`

![Screenshot](/img/LIBEV%20500b.png)

- Halaman 20KB

`ab -n 10000 -c 10000 http://127.0.0.1:3033/20KB.html`

![Screenshot](/img/LIBEV%2020KB.png)

## Hasil dalam Tabel

|| Apache  | Nginx  | Libev Program  |
|---|---|---|---|
|  Response Time 500b | 6 |  186 | 148  |
|  Response Time 20KB |  6 |  241 | 145.0 |
|  Memory Usage 500b |  1.2% | 0.1%  | 5.7%  |
|  Memory Usage 20KB   | 1.6%  | 0.1%  |  9.7% |
