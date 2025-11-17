; freverse.asm
; Odwraca zawartość pliku podanego jako argument.


%define SYS_EXIT    60    ; Kod syscall dla exit
%define SYS_OPEN     2    ; Kod syscall dla open
%define SYS_CLOSE    3    ; Kod syscall dla close
%define SYS_FSTAT    5    ; Kod syscall dla fstat
%define SYS_MMAP     9    ; Kod syscall dla mmap
%define SYS_MUNMAP  11    ; Kod syscall dla munmap
%define SYS_MSYNC   26    ; Kod syscall dla msync

%define O_RDWR      02    ; Flaga: odczyt i zapis
%define O_CLOEXEC 0x80000 ; Flaga: zamknij przy exec
%define PROT_READ   1     ; Flaga mmap: dostęp do odczytu
%define PROT_WRITE  2     ; Flaga mmap: dostęp do zapisu
%define MAP_SHARED  1     ; Flaga mmap: mapowanie dzielone
%define MS_SYNC     4     ; Flaga msync: synchroniczny zapis

section .bss
    statbuf resb 144      ; Bufor na strukturę stat

section .text
global _start

_start:
    ; Sprawdzamy liczbe argumentow
    mov rsi, [rsp]        ; Pobierz argc
    cmp rsi, 2            ; Sprawdzamy czy sa dwa argumenty
    jne .error_exit       ; Jesli nie, konczymy z bledem

 
    mov rsi, [rsp+16]     ; Pobieramy nazwe pliku

    ; Otwieramy plik
    mov rdi, rsi          ; Ustawiamy nazwe pliku jako pierwszy argument
    mov rsi, O_RDWR | O_CLOEXEC ; Flagi: odczyt/zapis + zamknij przy exec
    mov rdx, 0            
    mov rax, SYS_OPEN     ; Wywolujemy syscall open
    syscall
    test rax, rax         ; Sprawdzamy czy fd jest ujemne
    js .error_exit        ; Jesli blad, konczymy z bledem
    mov r12, rax          ; Zapisz deskryptor pliku w r12
    
    ; fstat
    mov rdi, r12          ; Ustawiamy deskryptor
    mov rsi, statbuf      ; Ustawiamy wskaznik na bufor stat
    mov rax, SYS_FSTAT    ; Wywolujemy syscall fstat
    syscall
    test rax, rax         ; Sprawdzamy kod powrotu
    js .close_error_exit  ; Jesli blad, zamykamy i konczymy

    ; Odczytujemy rozmiar pliku 
    mov rax, [statbuf+48] ; Odczytujemy st_size
    mov r13, rax          ; r13 = rozmiar pliku

    cmp r13, 2            ; Sprawdzamy czy rozmiar < 2 bajtów
    jb .close_success_exit ; Jesli tak, konieczymy bez zmian

    ; mmap
    mov rdi, 0            ; Adres
    mov rsi, r13          ; Ustawiamy dlugosc mapowania jako rozmiar pliku
    mov rdx, PROT_READ | PROT_WRITE ; Dostep: odczyt + zapis
    mov r10, MAP_SHARED   ; Flaga: mapowanie dzielone
    mov r8,  r12          ; Deskryptor pliku
    mov r9,  0            ; Offset ustawiamy na 0-poczatek pliku
    mov rax, SYS_MMAP     ; Wywolujemy syscall mmap
    syscall
    test rax, rax         ; Sprawdzamy czy adres jest bledny
    js .close_error_exit  ; Jesli blad, zamykamy i konczymy
    mov r14, rax          ; r14 = adres mapowania

    ; Odwracanie
    mov rsi, r14          ; Wskaznik na poczatek danych (rsi = start)
    mov rdi, r14          ; Kopiujemy adres startowy
    add rdi, r13          ; Przesuwamy wskaznik na koniec danych (r13 = rozmiar pliku)
    dec rdi               ; Ustawiamy rdi na ostatni bajt (rdi = ostatni bajt)

.reverse_loop:
    cmp rsi, rdi          ; Porownujemy wskazniki (start >= end)
    jge .sync_unmap       ; Jesli tak, konczymy petle

    ; Zamiana bajtow
    mov al, [rsi]         ; Pobieramy bajt z poczatku (rsi = start)
    mov bl, [rdi]         ; Pobieramy bajt z końca (rdi = koniec)
    mov [rsi], bl         ; Zapisujemy bajt koncowy na poczatku
    mov [rdi], al         ; Zapisujemy bajt poczatkowy na koncu

    inc rsi               ; Przesuwamy wskaznik poczatku w prawo
    dec rdi               ; Przesuwamy wskaznik konca w lewo
    jmp .reverse_loop     ; Kontynuujemy petle

.sync_unmap:
    ; synchronizujemy zmiany
    mov rdi, r14          ; Adres mapowania
    mov rsi, r13          ; Dlugosc danych
    mov rdx, MS_SYNC      ; Flaga: synchroniczny zapis
    mov rax, SYS_MSYNC    ; Wywolujemy syscall msync
    syscall
    test rax, rax         ; Sprawdzamy kod powrotu
    js .munmap_close_error_exit ; Jesli blad, zamykamy i konczymy

    ; zwalniamy mapowanie
    mov rdi, r14          ; Adres mapowania
    mov rsi, r13          ; Dlugosc
    mov rax, SYS_MUNMAP   ; Wywolujemy syscall munmap
    syscall
    test rax, rax         ; Sprawdzamy kod powrotu
    js .close_error_exit  ; Jesli blad, zamykamy i konczymy

.close_success_exit:
    ; zamykamy plik
    mov rdi, r12          ; Deskryptor pliku
    mov rax, SYS_CLOSE    ; Wywolujemy syscall close
    syscall
    test rax,rax          ; Sprawdamy wynik 
    js .error_exit        ; Jesli blad, konczymy z bledem
    
    mov rdi, 0            ; Kod wyjscia: 0
    mov rax, SYS_EXIT     ; Wywolujemy syscall exit
    syscall

.munmap_close_error_exit:
    ; blad msync
    mov rdi, r14          ; Adres mapowania
    mov rsi, r13          ; Dlugosc
    mov rax, SYS_MUNMAP   ; Wywolujemy munmap
    syscall

.close_error_exit:
    ; zamykamy plik przed wyjscieem
    mov rdi, r12          ; Deskryptor pliku
    mov rax, SYS_CLOSE    ; Wywolujemy syscall close
    syscall

.error_exit:
    ; konczymy z bledem
    mov rdi, 1            ; Kod wyjscia: 1
    mov rax, SYS_EXIT     ; Wywolujemy syscall exit
    syscall
