.code						;required to use procedures in .inc file
SPLASHSCREEN proc

		call drawsplash			;DRAW THE SPLASHSCREEN HERE AND PLAY SOME MUSIC
		call introsound
		
		call getusername
		call mainscreenui
		
		ret
SPLASHSCREEN endp
;==========================================;
outro proc					
		
		call drawoutro
		call introsound
		
		mov ah,0				;change to text mode
		mov al,03h
		int 10h
		
		mov ax,4c00h
		int 21h
outro ENDP
;==============================================;
drawsplash proc
		; Graphic mode
		mov ax, 13h
		int 10h

		; Process BMP file
		call OpenFileintro
		call ReadHeader
		call ReadPalette
		call CopyPal
		call CopyBitmap
		RET
drawsplash endp
;================================================;
introsound proc
	
		mov ah, 3Dh
		xor al, al
		lea dx, [filenamesound]
		int 21h
		
		
		
		mov [filehandlesounds], ax
		;call br
		mov al, 90h
		out 43h, al
		in al, 61h
		or al, 3
		out 61h, al
		cli
		;mov ax, 0
		
	totalloop:
		mov ax, counter
		dec ax
		mov counter,ax
		mov ax, 0
		call read ; Read file
		out 42h, al ; Send data
		mov bx, ax
		mov cx, [delaysound]
		
	portloop:
		loop portloop
		mov ax, bx
		out 42h, ax ; Send data
		mov cx, [delaysound]
		
	rloop:
		loop rloop
		call read			
		
		cmp counter,0
		jne totalloop
	endsong:
		
		sti
		mov al, 86h
		out 43h, al
		; Close the file
		mov ah, 3Eh
		mov bx, [filehandlesounds]	
		int 21h		
	 ret
introsound endp
;================================
read proc  ; Read next sample
	push bx
	push cx
	push dx
	mov ah, 3Fh
	mov bx, [filehandlesounds]
	mov cx, 1
	lea dx, [Buffer]
	int 21h
	mov al, [Buffer]
	xor ah, ah
	mov bx, 54
	mul bx
	
	shr ax, 1
	shr ax, 1
	shr ax, 1
	shr ax, 1
	shr ax, 1
	shr ax, 1
	shr ax, 1
	shr ax, 1

	pop dx
	pop cx
	pop bx

	ret
read endp
;================================
drawoutro proc
		; Graphic mode
		mov ax, 13h
		int 10h

		; Process BMP file
		call OpenFileouttro
		call ReadHeader
		call ReadPalette
		call CopyPal
		call CopyBitmap
		ret
drawoutro endp
;================================
WAIT5S PROC					;WAITS 5 SECONDS AFTER THE SPLASHSCREEN AND IT WORKS!!!!!!
		MOV AH,86H
		MOV DX,4B40H		;5 SECONDS IN HEXA IS 4C4B0 SO PUT IT IN CX,BX
		MOV CX,004CH
		INT 15H
		RET
WAIT5S ENDP
;================================
getusername proc
		
		; Graphic mode
		mov ax, 0eh
		int 10h
		
		call clearinputbuffer
		
		mov ah,2          		;Move Cursor to upper middle of screen
		mov dx,0A1Fh      		
		int 10h           		
		
		mov ah, 9				;Display 'Please enter your name:'
		mov dx, offset SPLASH1	
		int 21h 
		
		mov ah,2          		;Move Cursor to lower middle of screen
		mov dx,0E1Dh      		
		int 10h           		

		mov ah, 9				;Display 'press enter to continue'
		mov dx, offset SPLASH2	
		int 21h 
		
		mov ah,2          		;Move Cursor, to middle of screen
		mov dx,0C1Fh      		
		int 10h           		
		
		mov ah, 9				;Display 'Player 1:'
		mov dx, offset NAME1	
		int 21h
		
		mov ah,0AH       		;Read name from keyboard
		mov dx,offset PLAYER1NAME                  
		int 21h
		;;;;;;;;;;;;;;;;;this part clears only the space where the name is;;;;;;;;;;;;;;;;;;
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		MOV AL,0			;draw black rectangle over text
		MOV AH,0CH			;interrupt condition
		MOV BH,0
		MOV DX,96
		cleartxt1:
			MOV CX,312
			
			cleartxt2:
				INT 10H
				INC CX
				CMP CX,432	
			JNZ cleartxt2
			INC DX
			CMP Dx,104
		JNZ cleartxt1
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		
		mov ah,2          		;Move Cursor, to middle of screen
		mov dx,0C1Fh      		
		int 10h     
		
		mov ah, 9				;Display 'Player 2:'
		mov dx, offset NAME2	
		int 21h
		
		mov ah,0AH       		;Read name from keyboard
		mov dx,offset PLAYER2NAME                  
		int 21h
		
		ret
getusername endp
;================================
clearinputbuffer proc
		
		mov al,0
		mov ah,0CH
		int 21h 			;this clears the buffer, then moves al to ah and executes the int 21h again only if al=1,6,7,8,0AH
		
		ret
clearinputbuffer endp
;================================
OpenFileintro proc 

    ; Open file

    mov ah, 3Dh
    xor al, al
    mov dx, offset filenameintro
    int 21h

    jc openerrorintro
    mov [filehandle], ax
    ret

    openerrorintro:
    mov dx, offset ErrorMsg
    mov ah, 9h
    int 21h
    ret
OpenFileintro endp 
;================================
OpenFileouttro proc 

    ; Open file

    mov ah, 3Dh
    xor al, al
    mov dx, offset filenameouttro
    int 21h

    jc openerrorouttro
    mov [filehandle], ax
    ret

    openerrorouttro:
    mov dx, offset ErrorMsg
    mov ah, 9h
    int 21h
    ret
OpenFileouttro endp 
;================================
ReadHeader proc 

    ; Read BMP file header, 54 bytes

    mov ah,3fh
    mov bx, [filehandle]
    mov cx,54
    mov dx,offset Header
    int 21h
    ret
ReadHeader    endp 
;================================
ReadPalette    proc 

    ; Read BMP file color palette, 256 colors * 4 bytes (400h)

    mov ah,3fh
    mov cx,400h
    mov dx,offset Palette
    int 21h
    ret
ReadPalette endp 
;================================
CopyPal proc 

    ; Copy the colors palette to the video memory
    ; The number of the first color should be sent to port 3C8h
    ; The palette is sent to port 3C9h

    mov si,offset Palette
    mov cx,256
    mov dx,3C8h
    mov al,0

    ; Copy starting color to port 3C8h

    out dx,al

    ; Copy palette itself to port 3C9h

    inc dx
    PalLoop:

    ; Note: Colors in a BMP file are saved as BGR values rather than RGB.

    mov al,[si+2] ; Get red value.
	shr al, 1
	shr al, 1

    ; value is 63. Therefore dividing by 4.

    out dx,al ; Send it.
    mov al,[si+1] ; Get green value.
	shr al, 1
	shr al, 1
    out dx,al ; Send it.
    mov al,[si] ; Get blue value.
	shr al, 1
	shr al, 1
    out dx,al ; Send it.
    add si,4 ; Point to next color.

    ; (There is a null chr. after every color.)

    loop PalLoop
    ret
CopyPal endp 
;================================
CopyBitmap proc 

    ; BMP graphics are saved upside-down.
    ; Read the graphic line by line (200 lines in VGA format),
    ; displaying the lines from bottom to top.

    mov ax, 0A000h
    mov es, ax
    mov cx,200
    PrintBMPLoop:
    push cx

    ; di = cx*320, point to the correct screen line

    mov di,cx

    shl cx,1
    shl cx,1
    shl cx,1
    shl cx,1
    shl cx,1
    shl cx,1

    shl di,1
    shl di,1
    shl di,1
    shl di,1
    shl di,1
    shl di,1
    shl di,1
    shl di,1
	
    add di,cx

    ; Read one line

    mov ah,3fh
    mov cx,320
    mov dx,offset ScrLine
    int 21h

    ; Copy one line into video memory

    cld 

    ; Clear direction flag, for movsb

    mov cx,320
    mov si,offset ScrLine
    rep movsb 

    ; Copy line to the screen
    ;rep movsb is same as the following code:
    ;mov es:di, ds:si
    ;inc si
    ;inc di
    ;dec cx
    ;loop until cx=0

    pop cx
    loop PrintBMPLoop
    ret
CopyBitmap endp 
;================================
