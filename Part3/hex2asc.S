  # hexmain.S
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
	.globl hex2asc

hex2asc:
	li	a0, 17	# test number (from 0 to 15)     #Input till hexasc funktionen
	
	addi    sp,sp,-4                                                            
	sw      ra,0(sp)                          
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	# write a0 to stdout                          
	ecall

	lw      ra,0(sp)                                        
	addi    sp,sp,4                                                          
	jr      ra	

  # You can write your own code for hexasc here 
  
hexasc:
       andi a0, a0, 0xF            # F = 111, nollställer alla bitar utom  de sista 4 bitarna
       li t0, 10                   # Sätts till 10 för avgöra bokstäver eller siffror.
       blt a0, t0, numbers         # a0 < t0 gå till numbers 
       bge a0, t0, letters         # a0 >= 10 gå till letters
       
       
numbers:
        addi a0, a0, 0x30           # om a0 = 5 blir a0= 0x30 + 0x5 = 0x35 = '5' osv
        ret                         # returnera                            
       
        
letters:
        addi a0, a0, -10           #Lägger till -10 till a0, ex: om a0 = 11 blir a0 = 11-10 = 1, a0 blir ett index
        addi a0, a0, 0x41          #Adderar 0x41(A) till a0 så a0 = 0x1 + 0x41 = 0x42 = B osv
        ret                        #Returnera
       
       
       
     
       
