
SECTION .bss

    array resq 4000

SECTION .text

; Argumenty:
; RDI -> y
; RSI -> x
; RDX = n 

global ninv

ninv:

; Odkladamy na stos callee-saved rejestry.
    push rbp
    mov  rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov rcx, rdx                ; rcx = n
    shr rcx, 6                  ; rcx = numwords = n/64
    mov rax, rcx                ; rax = numwords
    dec rax                     ; rax := i = numwords-1

; Sprawdzamy czy dzielnik jest wiekszy lub rowny 2^(n-1)
    mov r8, rax                 ; r8 := i = numwords-1 = indeks ostatniego slowa
    mov r9, [rsi + r8*8]        ; r9 = dzielnik[numwords-1]
    mov r10, r9                 ; r10 = dzielnik[numwords-1]
    shr r9, 63
    shl r9, 63                  ; r9 = najstarszy bit slowa dzielnik[numwords-1]               
    test r9, r9
    jz .main_algorithm          ; Jesli najstarszy bit jest zerem, przechodzimy do glownego algorytmu.
                                ; W przeciwnym wypadku weryfikujemy czy wynik to 1 czy 2.
    not r9                      ; r9 = not r9
    test r10, r9
    jnz .equals_one             ; Jesli jakis bit poza najstarszym jest niezerowy, to dzielnik > od 2^(n-1)
                                ; wiec wynik to 1
    dec r8                      ; r8 = i-1
.check_divisor:
    cmp r8, 0                     
    jl .equals_two              ; Jesli wszystkie pozostale slowa sa zerowe, to wynik rowna sie 2.
    mov r9, [rsi + r8*8]        ; r9 = dzielnik[i]
    test r9, r9
    jnz .equals_one             ; Jesli kolejne slowo dzielnika jest niezerowe, to dzielnik > 2^(n-1).
    dec r8                      ; r8 = i-1
    jmp .check_divisor          
.equals_one:
    mov qword [rdi], 1          ; y[0] = 1
    jmp .zeroes
.equals_two:
    mov qword [rdi], 2          ; y[0] = 2
.zeroes:
    mov r8, 1                   ; r8 := k = 1
.zeroes_loop:
    cmp rcx, r8                 ; Sprawdzamy czy numwords < k.
    jz .end                     ; Jesli k = numwords, to konczymy algorytm.
    mov qword [rdi + r8*8], 0   ; dla 1 <= k < numwords y[k] = 0 
    inc r8                      ; k++
    jmp .zeroes_loop

; Jeśli dzielnik jest mniejszy lub rowny 2^(n-1), to zaczynamy wlasciwy algorytm znajdowania odwrotnosci.
.main_algorithm:

; Na poczatek szukamy ostatniego niezerowego slowa w x.
.find_positive:
    mov r8, [rsi + rax*8]       ; r8 = dzielnik[i]
    test r8,r8
    jnz .last_positive          ; Jesli znalezlismy zapisujemy indeks.
    dec rax                     ; i--
    jmp .find_positive
.last_positive:
    mov r15, rax                ; r15 = indeks ostatniego dodatniego slowa
    inc r15                     ; r15 = liczba istotnych slow dzielnika = relwords

;Ustawiamy 2 w reszta[0] oraz 0 w reszta[1]...reszta[relwords].
    lea rax, [rel array]        ; rax = adres reszty
    mov r8, r15                 ; r8 = relwords 
    mov r9, rax                 ; r9 = adres reszty
    mov qword [r9], 2           ; Gdy dzielnik ma conajmniej dwie cyfry, to pierwszą cyfrą wyniku będzie 0,
                                ; a pierwsza cyfra zapisana na n bitch wyniku będzie pochodziła z przesuniętej reszty.
                                ; Zatem możemy zacząć dzielenie od 1 przesuniętego w lewo, czyli 2.
    add r9, 8
    xor rcx, rcx                ; rax = i = 0  
.set_remaining_loop:
    mov [r9], rcx               ; reszta[i] = 0
    add r9, 8                   ; reszta++
    dec r8                      ; i--
    jnz .set_remaining_loop     ; Kontynuuj, jeśli i > 0.


; Przygotowujemy wskaźniki do włwasciwej czesci dzielenia.
    mov r8, r15                 ; r8 = relwords
    mov r9, rax                 ; r9 = adres reszty 
    mov r10, rdi                ; r10 = adres wyniku = &y

; Główna pętla dzielenia pisemnego
    mov rbx, rdx                ; rbx = n
    dec rbx                     ; rbx := j = n - 1 (za pomocą j indeksujemy ustawiane bity wyniku)
.div_loop:
    cmp rbx, 0                  ; Porownaj indeks bitu z liczbą bitów n,
    jl .end                     ; jesli skonczyly sie bity to koniec.

; Porównujemy resztę z dzielnikiem.
    mov r11, rsi                ; r11 = adres dzielnika
    mov r10, r15                ; r10 := i = relwords
    mov rax, [r9 + r10*8]
    test rax, rax               ; Sprawdzamy czy reszta[relwords] > 0.
    jnz .set_greater_or_equal   ; Jesli tak, to reszta > dzielnik i przechodzimy do dalszego etapu dzielenia,
                                ; jesli nie, to porównujemy k-te słowo reszty z k-tym słowem dzielnika.
    dec r10                     ; r10 = i-1 
.compare_words:
    cmp r10, 0                  ; Sprawdzamy czy i >= 0.
    jl .set_greater_or_equal    ; Jesli wszystkie slowa sa rowne to reszta = dzielnik           
    mov r13, [r11 + r10*8]      ; r13 = dzielnik[i]
    cmp [r9 + r10*8], r13       ; Sprawdzamy czy i-te słowo reszty jest mniejsze, większe czy równe i-temu słowu dzielnika.
    jb .set_less                ; Jesli reszta < dzielnik, to przeskocz do set_less.
    ja .set_greater_or_equal    ; Jesli reszta > dzielnik to przeskocz do set_greater.
    dec r10                     ; r10 = i-1      
    jmp .compare_words          ; Jesli i >= 0 oraz słowa sa równe, to kontynuujemy porownywanie.

.set_greater_or_equal:
    mov r13, 1                  ; r13 = 1 (reszta >= dzielnik)
    jmp .after_compare
.set_less:
    xor r13, r13                ; r13 = 0 (reszta < dzielnik)

; Ustawiamy bit w wyniku i odpowiednio modyfikujemy resztę.
.after_compare:
    mov rax, rbx                ; rax = j
    mov r14, rbx                ; r14 = j
    shr r14, 6                  ; r14 = indek_slowa = j / 64
    and rax, 63                 ; rax = indeks_bitu = j % 64
    test r13, r13
    jz .mem_clear
    mov r13, [rdi + r14 * 8]
    bts r13, rax                ; Ustaw bit indeks_bitu w y[indeks_slowa] na 1.
    mov [rdi + r14 * 8], r13
    jmp .subtract               ; Jesli wpisalismy 1, to odejmujemy dzielnik od reszty.
.mem_clear:
    mov r13, [rdi + r14 * 8]
    btr r13, rax                ; Ustaw bit indeks_bitu w y[indeks_slowa] na 0.
    mov [rdi + r14 * 8], r13
    jmp .shift_left             ; Jesli wpisalismy 0, to przesuwamy reszte.

; Odejmujemy dzielnik od reszty (wielosłowowo).
.subtract:
    mov r13, r15                ; r13 = relwords
    xor r10, r10                ; r10 := i = 0, CF = 0
.sub_loop:
    mov rax, [r11 + r10*8]      ; rax = dzielnik[i]
    sbb [r9 + r10*8], rax       ; reszta[i] = reszta[i] - dzielnik[i] - CF
    inc r10                     ; i++  
    dec r13                     ; relwords--
    jnz .sub_loop               ; Jesli i < relwords, to kontynuujemy.
.sub_last_bit:
    mov rax, [r9 + r10*8]       ; rax = reszta[relwords]
    sbb rax, 0                  ; Odejmujemy CF od najstarszego slowa reszty, gdy wystapila pożyczka.
    mov [r9 + r10*8], rax       ; Przywracamy wlasciwa wartosc ostatniego slowa reszty.

; Przesuń resztę w lewo o 1 bit (wielosłowowo).
.shift_left:
    xor r10, r10                ; r10 = i = 0
    mov r13, r15                ; r13 = relwords 
.shift_loop:
    rcl qword [r9 + r10*8], 1   ; Przesuwamy slowo reszty, ustawiajac CF zgodnie z najstarszym bitem.
    inc r10                     ; i++
    dec r13
    jnz .shift_loop

;Dodajemy ostatnie carry i przechodzimy do obsługi kolejnego bitu wyniku.
.next_bit:
    adc qword [r9 + r10*8], 0   ; Dodajemy carry do ostatniego slowa reszty.
    dec rbx                     ; Zmniejszamy indeks ustalanego bitu wyniku.
    jmp .div_loop               ; Powtarzamy dzielenie.

; Zdejmujemy ze stosu callee-saved rejestry i konczymy dzialanie funkcji.
.end:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret