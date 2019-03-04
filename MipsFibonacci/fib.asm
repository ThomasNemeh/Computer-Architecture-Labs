#Author: Thomas Nemeh
#Program in mips assembly language that computes a sequence of fibonacci numbers and displays it in decimal and hexadecimal notation.
#The number of one bits in each number of the sequence is also included.

.text
    main:
        #prompt user to enter the first number in the sequence
        li $v0, 4
        la $a0, prompt
        syscall
        
        #get the first number in the sequence
        li $v0, 5
        syscall
        
        #store result $t0 
        move $t0, $v0
        
        #prompt user to enter the second number in the sequence
        li $v0, 4
        la $a0, prompt2
        syscall
        
        #get the second number in the sequence
        li $v0, 5
        syscall
        
        #store result $t1
        move $t1, $v0
        
        #prompt user to enter the number of iterations
        li $v0, 4
        la $a0, prompt3
        syscall
        
        #get the number of iterations
        li $v0, 5
        syscall
        
        #store result in $t2
        move $t2, $v0
        
        #skip a line
        li $v0, 4
        la $a0, newln
        syscall
        
        #print first element in the sequence
        move $a1, $t0
        jal println
        
        #print the second element in the sequence
        move $a1, $t1
        jal println
        
        #store counter variable for our loop
        li $t3 3
        
        while:
            #continue loop if $t2 >= $t3
            bgt $t3, $t2, exit
            
            #calculate new number in sequence and update registers
            move $t4, $t1
            add $t1, $t1, $t0
            move $t0, $t4
            
            #print new element in the sequence
            move $a1, $t1
            jal println
            
            #subtract counter
            sub $t2, $t2, 1
        
            #jumpt to top of loop
            j while
            
        exit:
        
    	#tell the system that the program is done
    	li $v0, 10
    	syscall
   
        #prints number of fibonacci sequence in decimal and hex. Prints number of one bits.
        println:
            #store return address for nested function
            addi $sp, $sp, -4
            #sw $s0, 0($sp)
            sw $ra, 0($sp)   
            
            #print element in decimal form
            li $v0, 1
            move $a0, $a1
            syscall
            
            #print space
            li $v0, 4
            la $a0, space
            syscall
            
            #print element in hex
            li $v0, 34
            move $a0, $a1
            syscall
            
            #print space
            li $v0, 4
            la $a0, space
            syscall
            
            #get one bits in 32-bit number
            jal countOneBits
            #resume program from correct return address
            lw $ra, 0($sp)
            #print return value
            li $v0, 1
            move $a0, $v1
            syscall
            
            #print new line
            li $v0, 4
            la $a0, newln
            syscall
            
            #resume main function
            jr $ra
    
    #counts one bits in the 32-bit number in register $a0. Does not count the sign bit.
    countOneBits:
        #load counter for our loop in register $t4
        li $t4, 29
        #load ones counter in register $t5
        move $t5, $zero
            
        while2:
            #exit loop if register $t4 is less than 0
            bgt $zero, $t4, exit2
            
            #get least significant bit of our number
            andi $t6, $a1, 0x01
            
            #if the least significant bit is 1, then increment our ones counter
            beq $t6, $zero, endif
            add $t5, $t5, 1
            endif: 
                #right shift our number
                srl $a1, $a1, 1
                
                #subtract our loop counter and jump to top of loop
                sub $t4, $t4, 1
                j while2
        exit2:
            #store return value
            move $v1, $t5
            #resume previous function
            jr $ra
        

   
    
    
.data
    prompt: .asciiz "Enter the first number in the sequence: \n"
    prompt2: .asciiz "Enter the second number in the sequence: \n"
    prompt3: .asciiz "Enter the number of iterations: \n"
    
    space: .asciiz "   "
    newln: .asciiz "\n"
