vypis   MACRO t     ;makro pre vypis retazca s offsetom t v pamati
        push dx
        push ax
        mov dx, offset t
        mov ah, 09h
        int 21h
        pop ax
        pop dx
        endm
clscr   MACRO       ;makro pre vycistenie obrazovky
        push ax
        mov ax, 0003h
        int 10h
        pop ax
        endm
fopen   MACRO t     ;makro pre otvorenie suboru ktoreho nazov je na offsete t v pamati
        push dx
        mov ah, 3dh
        mov al, 0
        mov dx, offset t
        int 21h
        pop dx
        endm
fclose  MACRO       ;makro pre zatvorenie suboru ktoreho file handle je v bx
        push ax
        mov ah, 3eh
        int 21h
        pop ax
        endm
pause   MACRO       ;makro pre vstup znaku z klavesnice sluziace ako pauza
        push ax
        mov ah, 1
        int 21h
        pop ax
        endm
newline MACRO       ;makro pre vypisanie noveho riadku
        push ax
        push dx
        mov dl, 13
        mov ah, 02h
        int 21h
        mov dl, 10
        mov ah, 02h
        int 21h
        pop dx
        pop ax
        endm
space   MACRO       ;makro pre vypisanie medzery
        push ax
        push dx
        mov dl, 32
        mov ah, 02h
        int 21h
        pop dx
        pop ax
        endm