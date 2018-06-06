/*
Pour assembler:
	
	msp430-gcc -mmcu=msp430fg4618 -mdisable-watchdog -o prog.elf multmsp430.s
	msp430-objdump -d prog.elf

	donne 192 bits, dont 96 dans le coeur de boucle.
	La boucle fait est exécutée 5 fois, chaque fois en faisant 5 ou 6 instructions. Chaque instruction transfère 32 bits (16 A, 16 D). Donc le nombre de bits transmis lors du coeur de boucle est au min de 5*5*32= bits.
Plus encore 5*32 pour l'init, total 960 bits	
	
0000312c <main>:
    312c:	3a 40 11 00 	mov	#17,	r10	;#0x0011
    3130:	7b 40 2a 00 	mov.b	#42,	r11	;#0x002a
    3134:	0c 43       	clr	r12		

00003136 <nonzero>:
    3136:	0a 10       	rrc	r10		
    3138:	01 28       	jnc	$+4      	;abs 0x313c
    313a:	0c 5b       	add	r11,	r12	

0000313c <next>:
    313c:	0b 5b       	rla	r11		
    313e:	0a 93       	tst	r10		
    3140:	00 20       	jnz	$+2      	;abs 0x3142

00003142 <loop>:
    3142:	ff 3f       	jmp	$+0      	;abs 0x3142

	*/
.section .init9

main:


  mov #17, r10   
  mov.b #42, r11  
	
  mov #0, r12
nonzero:	
  rrc r10, r10
  jnc next
  add r11, r12
next:	
  rla r11, r11
  cmp #0, r10
  jnz nonzero


loop:	
	jmp	loop

	
