;-------------------------------
;-------- Viktor Beno ----------
;--------    FIIT     ----------
;--------   SPaASM    ----------
;--------  zadanie 1  ----------
;--------   uloha 3   ----------
;-------------------------------
;ULOHA: Najst znaky s maxim?lnou
; hodnotou a vypisat pozicie ich
; vyskytov.
;-------------------------------
;BONUS: Vypisanie dlzky suboru
; inak ako precitanim celeho
; suboru.
;-------------------------------
;OBMEDZENIA:  
;   - spravna dlzka suboru iba
; pri dlzke < 65 536
;   - neprehladne vypisovanie
; pozicii znakov s maximalnou
; hodnotou
;-------------------------------

include makra.asm                   ;include suboru s makrami
        
zas SEGMENT stack                   ;zasobnik pre uchovavanie udajov registrov
    dw 64 dup(?)
zas ends

data SEGMENT                        ;datovy segment s potrebnymi retazcami a miestami pre data
    menu db 'Vyber cislo moznosti:',13,10
         db '1 - zadaj meno suboru',13,10
         db '2 - vypisat obsah suboru',13,10
         db '3 - vypisat dlzku suboru',13,10
         db '4 - vykonat pridelenu ulohu',13,10
         db '5 - ukoncit program',13,10,'$'
    
    datumT  db 'Dnesny datum: $'
    datum   db 'dd.mm.rrrr',13,10,'$'
    casT    db 'Systemovy cas: $'
    cas     db '00:00:00',13,10,13,10,'$' 
    
    readIn      db 100
    readPocet   db 0
                db 100 dup(0)
    subor       db 100 dup(0)
    
    buffer      db 1025 dup('$')
    
    znak        db 0
    maxZnak     db 0,13,10,'$'
    maxZnakT    db 'Znak s maximalnou hodnotou v subore: ','$'
    maxZnakP    db 'Pozicie tohto znaku v subore:',13,10,'$'
    
    OtvorenieErrorMsg1 db 'Subor nenajdeny!',13,10, '$'
    OtvorenieErrorMsg2 db 'Pristup odmietnuty!',13,10, '$'
    OtvorenieErrorMsg3 db 'Neplatne cislo funkcie!',13,10, '$'
    OtvorenieErrorMsg4 db 'Mod otvorenia neplatny!',13,10, '$'
    OtvorenieErrorMsg5 db 'Neznama chyba!',13,10, '$'
         
DATA ENDS

code SEGMENT
    assume ds: data, cs: code
   
    start:
        mov ax, seg data            ;inicializovanie
        mov ds, ax
        
ShowMenu:                           ;navestie pre zobrazenie menu
        clscr                       ;vycistenie obrazovky
            
        lea bx, datum               ;vypisanie datumu
        call get_date
        vypis datumT
        vypis datum
        
        lea bx, cas                 ;vypisanie casu
        call get_time
        vypis casT
        vypis cas
        
        vypis menu                  ;vypisanie menu
        
        mov ah, 1                   ;funkcia na ziskanie znaku z klavesnice
        int 21h                     ;nacitany znak je v al
        
        cmp al, '1'                 ;switch v jsi prevedeni
        jl ShowMenu
        cmp al, '5'
        jg ShowMenu
        
        cmp al, '1'
        je ZSubor
        cmp al, '2'
        je VSubor
        cmp al, '3'
        je DSuboru
        cmp al, '4'
        je Uloha
        cmp al, '5'
        je Koniec
                                    ;volania jednotlivych procedur
    ZSubor:
    
        call zadajSubor
        
        jmp ShowMenu
        
    VSubor:
        
        call vypisSubor
    
        jmp ShowMenu
        
    DSuboru:
        
        call dlzkaSuboru
    
        jmp ShowMenu
        
    Uloha:
    
        call maxZnaky
    
        jmp ShowMenu
        
    Koniec:                         ;ukoncenie programu
        mov ah,4ch
        int 21h
     
;----------- PROCEDURY -----------
        
   get_time proc                   ;procedura na ziskanie casu z funkcie

        push ax                     ;odlozenie ax do stacku  
        push cx                     ;odlozenie cx do stacku  

        mov ah, 2ch                 ;funkcia na ziskanie aktualneho casu
        int 21h                       

        mov al, ch                  ;presun do al register ch v ktorom su hodiny
        call convert                ;procedura na prevedenie bin cisla do ascii
        mov [bx], ax                ;presun hodiny v ascii do retazca na miesto hodin

        mov al, cl                  ;presun do al register cl v ktorom su minuty
        call convert                ;procedura na prevedenie bin cisla do ascii
        mov [bx+3], ax              ;presun minuty v ascii do retazca na miesto minut
                                           
        mov al, dh                  ;presun do al register dh v ktorom su sekundy
        call convert                ;procedura na prevedenie bin cisla do ascii
        mov [bx+6], ax              ;presun sekundy v ascii do retazca na miesto sekund
                                 
                                                      
        pop cx                      ;vrat hodnoty cx zo stacku
        pop ax                      ;vrat hodnoty ax zo stacku

        ret                         
   get_time endp
   
   get_date proc                    ;procedura na ziskanie datumu funguje podobne
                                    ;ako procedura get_time
        push ax
        push cx
        
        mov ah,2ah
        int 21h
        
        mov al,dl
        call convert
        mov [bx],ax
        
        mov al,dh
        call convert
        mov [bx+3],ax
        
        mov al,100
        xchg ax,cx
        div cl
        mov ch,ah
        call convert
        mov [bx+6],ax
        
        mov al,ch
        call convert
        mov [bx+8],ax 
        
        pop cx
        pop ax    

        ret
   get_date endp
   
   convert proc                     ;procedura na konvertovanie binarneho cisla
                                    ;v ax do ascii kodu
        push dx                      

        mov ah, 0                   ;AH=0
        mov dl, 10                  ;DL=10
        div dl                      ;AX=AX/DX          
        or ax, 3030h                ;konvertovanie hodnoty na ascii

        pop dx                     

        ret                       
   convert endp 
   
   zadajSubor proc                  ;procedura na ziskanie nazvu suboru a jeho
                                    ;ulozenie v pamati
        clscr
        push ax
        push bx
        push cx
        push dx
        
        lea bx, readPocet           ;nulovanie pamate kde sa nachadza nazov suboru
        mov ax, 200                 ;
nulovanie:                          ;
        inc bx                      ;
        mov [bx], 0                 ;
        dec ax                      ;
        cmp ax, 0                   ;
        jne nulovanie               ;
                
        lea dx, readIn              ;nacitanie zo vstupu
        mov ah, 0ah                 ;
        int 21h                     ;
        
        add dx,2                    ;kopirovanie nazvu suboru a zbavenie
        mov si, dx                  ;sa carriage return
        xor cx, cx                  ;
        xor ax, ax                  ;
        mov cl, readPocet           ;
        lea bx, subor               ;
znova:                              ;
        mov al, [si]                ;
        mov [bx], al                ;
        inc si                      ;
        inc bx                      ;
        LOOPNE znova                ;
        
        pop dx
        pop cx
        pop bx
        pop ax
        
        ret
    zadajSubor endp
    
    vypisSubor proc                 ;procedura na vypisanie obsahu suboru
                                    ;bez strankovania
        clscr
        
        fopen subor                 ;vyuzitie makra na otvorenie suboru
        
        mov bx, ax                  ;presunutie file handle do bx
        push bx                     ;odlozenie file handle do stacku
        
        jnc Vypisanie               ;error check pri otvoreni
        call otvorenieError         ;
        ret
Vypisanie: 

        lea bx, buffer              ;nulovanie buffera pred zaplnenim
        mov ax, 1024                ;
nulovanie2:                         ;
        mov [bx], '$'               ;
        inc bx                      ;   
        dec ax                      ;
        cmp ax, 0                   ;
        jne nulovanie2              ;

        mov cx, 1024                ;citanie max 1024 znakov zo suboru
        lea dx, buffer              ;do bufferu
        mov ah,3fh                  ;
        pop bx                      ;ziskanie file handle zo stacku
        int 21h                     ;
        
        vypis buffer                ;vypisanie buffera na obrazovku
        
        push bx                     ;odlozenie file handle do stacku
        
        cmp ax, 1024                ;ak precitalo maximalny pocet tak znovu citaj
        je Vypisanie                ;
        
        pause                       ;makro pre pauzu
        
        pop bx
        fclose                      ;makro pre zatvorenie suboru
        
        ret
    vypisSubor endp
    
    dividebyten proc                ;procedura na delenie cislom 10 s vymenou
                                    ;vysledok v dx a zvysok v ax
        push bx
        
        mov bx, 10
        xor dx, dx
        div bx
        xchg ax, dx
        
        pop bx
        
        ret
    dividebyten endp
    
    printcharacter proc             ;jednoducha procedura na vypisanie znaku v al
    
        push dx
        
        mov dl, al
        mov ah, 02h
        int 21h
        
        pop dx
        
        ret
    printcharacter endp
    
    printinteger proc               ;procedura vypise cislo v ax na obrazovku
    
        push cx
        push dx
        
        mov    cx, 0                ;counter cifier
loop1:  call   dividebyten          ;delenie 10 v ax bude zvysok
        inc    cx                   ;zvysenie countera
        add    ax, 48               ;pricitanie hodnoty 48 pre ascii znak
        push   ax                   ;odlozenie cislice
        mov    ax, dx               ;vysledok delenia v dx sa presunie do ax
        cmp    ax, 0                ;koniec delenia ?
        jne    loop1
loop2:  pop    ax                   ;ziskanie cislic zo stacku
        call   printcharacter       ;vypis cislic
        dec    cx                   ;znizenie countera cifier
        cmp    cx, 0                ;koniec vypisovania ?
        jne    loop2
        
        pop dx
        pop cx
        
        ret
    printinteger endp
    
    dlzkaSuboru proc                ;procedura na vypis dlzky suboru
                                    ;pomocou file pointeru
        clscr
        
        fopen subor
        
        mov bx, ax
        jnc pokracuj
        call otvorenieError
        ret
pokracuj:
        mov ah, 42h                 ;funkcia na premiestnenie file pointeru
        xor cx, cx
        xor dx, dx
        mov al, 2                   ;mod 2 a hodnota cx=0 pre ziskanie konca suboru (dlzky)
        int 21h
        
        fclose
        
        call printinteger           ;procedura na vypisanie dlzky ktora je v ax
        
        pause
        
        ret
     dlzkaSuboru endp
     
     maxZnaky proc                  ;procedura na zistenie znaku s maximalnou hodnotou
                                    ; a vypisanie pozicii tochto znaku v subore
        clscr
        
        lea si, maxZnak             ;vynulovanie maxZnaku
        mov [si], 0
        
        fopen subor
        
        mov bx, ax
        jnc znovu2
        call otvorenieError
        ret
        
                                    ;prejdi suborom a najdi najvacsi znak
znovu2:
        mov ah, 3fh                 ;citanie po jednom znaku
        mov cx, 1                   
        lea dx, znak       
        int 21h
        
        cmp ax, 0                   ;ak je koniec suboru tak ist na eof
        je  eof
        
        lea si, znak                ;nacitat do cl precitany znak
        mov cl, [si]
        
        lea si, maxZnak             ;nacitat do ch zatial maximalny znak
        mov ch, [si]
        
        cmp cl, ch                  ;ak je precitany znak mensi tak znova citat dalsi znak
        jl znovu2
        
        lea si, maxZnak             ;ak je precitany vacsi tak prepisat maximalny na tento precitany
        mov [si], cl
        jmp znovu2
        
                                    ;prejdi suborom a vypis pozicie tychto znakov
eof:    
        fclose

        vypis maxZnakT              ;vypisanie max znaku
        vypis maxZnak
        vypis maxZnakP
        
        fopen subor
        
znovu3:
        mov ah, 3fh                 ;citanie 1 znaku zo suboru
        mov cx, 1
        lea dx, znak
        int 21h
        
        cmp ax, 0                   ;ak je koniec suboru tak skoncit
        je  maxZnakKoniec
        
        lea si, znak                ;presunut nacitany znak a max znak do cl a ch
        mov cl, [si]                ; a porovnat ich
        lea si, maxZnak
        mov ch, [si]
        cmp cl, ch                  ;ak nie su rovnake tak citaj dalsi znak
        jne znovu3
        
        mov ah, 42h                 ;ziskat poziciu citaneho znaku do ax
        mov cx, 0
        mov dx, 0
        mov al, 1                   ;mod 1 a cx=0 zabezpeci file pointer na aktualny znak
        int 21h
        
        call printinteger
        
        space                       ;makro na vypis medzery medzi poziciami
        
        jmp znovu3
        
maxZnakKoniec:
        fclose
        
        pause
        
        ret
     maxZnaky endp
     
     otvorenieError proc            ;procedura na zistenie chyby pri otvarani suboru
                                    ;v ax bude cislo podla chyby
        cmp ax, 5
        je OtvorenieError2
        cmp ax, 2
        je OtvorenieError1
        cmp ax, 1
        je OtvorenieError3
        cmp ax, 12
        je OtvorenieError4
        
        jmp OtvorenieError5
        
OtvorenieError1:
        vypis OtvorenieErrorMsg1
        pause
        ret
OtvorenieError2:
        vypis OtvorenieErrorMsg2
        pause
        ret
OtvorenieError3:
        vypis OtvorenieErrorMsg3
        pause
        ret
OtvorenieError4:
        vypis OtvorenieErrorMsg4
        pause
        ret
OtvorenieError5:
        vypis OtvorenieErrorMsg5
        pause
        ret
    otvorenieError endp
        
code    ends
end start
 