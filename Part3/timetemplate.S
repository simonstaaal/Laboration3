# timetemplate.S
# Written 2015 by F Lundevall
# Ported 2024/06 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

#############################################################
# Choose the macro syntax for RARS or DTEK-V board.         #
# The syntax for RARS is probably due to its MIPS heritage. #
#############################################################
###################
# Macros for RARS #
###################
#.macro	PUSH (%reg)
#	addi	sp,sp,-4
#	sw	%reg,0(sp) 
#.end_macro

#.macro	POP (%reg)
#	lw	%reg,0(sp)
#	addi	sp,sp,4
#.end_macro
###################
# Macros for DTEK #
###################
#.macro	PUSH reg
#	addi sp,sp,-4
#	sw \reg,0(sp) 
#.endm

#.macro	POP reg
#	lw	\reg,0(sp)
#	addi	sp,sp,4
#.endm
#############################################################

	.data
	.align 2
mytime:	.word 	0x5957
timstr:	.asciz 	"text more text lots of text\0"
	.text
	.globl timetemplate, tick, time2string, delay, display_string

# Function for displaying a string with a newline at the end	
display_string:	
	li a7,4
	ecall
	li a0, 10
	li a7,11
	ecall
	jr ra
	
timetemplate:
	la	a0, timstr
	jal     display_string
	
	# wait a little
	li	a0, 2		# ms  
	jal	delay
	
	# call tick
	la 	a0, mytime
	jal	tick
	
	# call your function time2string
	la	a0, timstr
	la	t0, mytime
	lw	a1, 0(t0)
	jal	time2string
	
	# go back and do it all again
	j	timetemplate

	
# tick: update time pointed to by $a0
tick:	lw	t0, 0(a0)	# get time
	addi	t0, t0, 1	# increase
	andi	t1, t0, 0xf	# check lowest digit
	sltiu	t2, t1, 0xa	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x6	# adjust lowest digit
	
	andi	t1, t0, 0xf0	# check next digit
	sltiu	t2, t1, 0x60	# if digit < 6, okay
	bnez	t2, tiend
	addi	t0, t0, 0xa0	# adjust digit
	
	li	t3, 0xF
	slli	t3, t3, 0x8
	and	t1, t0, t3	# check minute digit
	addi	t3, x0, 0xA
	slli	t3, t3, 0x8
	slt	t2, t1, t3	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x600	# adjust digit - this one's okay, it's lower than 0x7FF 
	
	li	t3, 0xF
	slli	t3, t3, 0xC
	and	t1, t0, t3	# check last digit
	addi	t3, x0, 0x6
	slli	t3, t3, 0xC
	slt	t2, t1, t3	# if digit < 6, okay
	bnez	t2, tiend
	
	li	t3, 0xA
	slli	t3, t3, 0xC
	add	t0, t0, t3	# adjust last digit
tiend:	sw	t0,0(a0)	# save updated result
	jr	ra		# return

#########################################################
# Place for your functions: time2string, hex2asc, delay.#
#########################################################


#######################################################
#Hex2asc
######################################################
hexasc:
       andi a0, a0, 0xF                                                      # F = 111, ser till att de sista bitarna i a0 är de som släpps igenom resterande bitar sätts till 0.
       li t0, 10                                                             # Sätts till 10 för att kunna jämföra inom vilket fall a0 hamnar i, siffror eller bokstäver.
       blt a0, t0, numbers                                                   # Jämför a0 med t0, i detta fall är a0 mindre än 10 så numbers anropas och utför additinen där.  
       bge a0, t0, letters                                                   # Är a0 större än eller lika med 10? om så gå till letters och utför additionen där.
       
       
numbers:
        addi a0, a0, 0x30                                                   # om a0 = 5 blir a0= 0x30 + 0x5 = 0x35 = '5' osv
        ret                                                                 # returnera                            
       
        
letters:
        addi a0, a0, -10                                                   # Lägger till -10 till a0, så om a0 = 11 blir a0 = 11-10 = 1
        addi a0, a0, 0x41                                                  #Adderar 0x41 till a0 så a0 = 0x1 + 0x41 = 0x42 = B osv
        ret                                                                #Returnera
       

#######################################################
#Delay
######################################################

delay:
     li t1, 10000     #(4711) Laddar konstanten för for-loopen
     
while:
     blez a0,end           #Om ms <= 0 hoppa till end
     li t0, 0              #i = 0

forloop:
       bge t0, t1, forend  #om i >= t1 gå till forend
       addi t0, t0, 1      #annars öka t0(i) med 1
       j forloop           #Gå tillbaka till forloop 

forend:
       addi a0, a0, -1     # minska ms med -1
       j while             # gå till while

end:         
    jr ra     

#######################################################
#time2string
# [15:2] = M 10-tal  [11:8]= M ental
# [7:4] = S 10-tal   [3:0] = S ental
######################################################     
time2string:
            addi sp, sp, -16   #Reserverar 16 byte på stacken
            sw ra, 12(sp)      #Sparar ra på tacken
            sw a0, 8(sp)       #Sparar pekaren till timestr på stacken
            sw s0, 4(sp)
                       
            mv s0, a0          #Flyttar pekaren i a0 till s0 
            
            ############################
            #Minut 10-tal
            ############################
            srli t0, a1, 12        # Flyttar bit 15:12 till 3:0
            andi t0, t0, 0xF       #0:ar alla bitar utom de sista 4
            mv a0, t0              # Flyttar värdet i t0 till a0 , a0 används i hex2asc
            jal hexasc             #Anropar funktionen
            sb a0, 0(s0)           #Lagrar ascii-värdet i timestr
            
            
            ############################
            #Minut ental
            ############################
            srli t1, a1, 8        #Flyttar bit 11:8 till 3:0
            andi t1, t1, 0xF      #0:Ar alla bitar utom de sista 4
            mv a0, t1             #FLyttar t1 till a0   
            jal hexasc            #anropar funktionen
            sb a0, 1(s0)          #Lagrar ascii-värdet i timestr
           
            
            
            ############################
            #Kolon
            ############################
            
            li t2, 0x3a           #Lagrar asciivärdet för : i t2
            sb t2, 2(s0)          #Lägger in ascii-värdet i timestr
               
            
            ############################
            #Sekund 10-tal
            ############################
            srli t3, a1, 4        #Flyttar 7:4 till 3:0
            andi t3, t3, 0xF      #0:ar alla bitar utom sista 4
            mv a0, t3             #Flyttar t3 till a0
            jal hexasc            #Anropar funktionen
            sb a0, 3(s0)          #Lagrar ascii-värdet i timestr
            
            
            
            ############################
            #Sekund ental
            ############################           
            andi t4, a1, 0xF      #0:ar alla bitar utom de sista 4
            mv a0, t4             #Flyttar t4 till a0
            jal hexasc            #Anropar funktionen
            sb a0, 4(s0)          #Lagrar ascii-värdet i timestr
            
            
                   
            ############################
            #null-byte
            ############################       
            li t5, 0
            sb t5, 5(s0) #Avslutar strängen
            
            #Återställer stacken som den var innan time2string
            lw s0, 4(sp)
            lw a0, 8(sp)   #hämtar innehåll i ofset 8, dvs  a0
            lw ra, 12(sp)  #Hämtar innehållet i offset 12, ra
            addi sp, sp, 16  #Tar bort" 16 byte från stacken
            ret              #Returnera
            
            
          
            
