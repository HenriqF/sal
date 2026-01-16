mysys2:    pacman -S mingw-w64-x86_64-openssl
 
gcc sal.c readwrite.c -o sal -lssl -lcrypto
