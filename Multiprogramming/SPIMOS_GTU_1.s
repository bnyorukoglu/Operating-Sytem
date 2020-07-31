
.data
lbl2: .asciiz "Enter second number: "
lbl3: .asciiz "Enter third number:"
file1: .asciiz "LinearSearch.asm"
file2: .asciiz "BinarySearch.asm"
file3: .asciiz "Collatz.asm"
file:    .word file1, file2, file3

.text

main:
li $v0, 22  # call init
syscall


li $v0, 20 # call fork 
syscall
bne $a1,$zero,not_child # check wheter child
la $a0, file1  
li $v0, 19 
syscall  # call execv

not_child:         # WAiting for all children to die
sw $a0, 0($sp)
addi $sp, $sp, -4
li $v0, 20 # call fork 
syscall
bne $a1,$zero,not_child2 # check wheter child
la $a0, file2  
li $v0, 19 
syscall  # call execv

not_child2:
sw $a0, 0($sp)
addi $sp, $sp, -4
li $v0, 20 # call fork 
syscall
bne $a1,$zero,not_child3 # check wheter child
la $a0, file3  
li $v0, 19 
syscall  # call execv

not_child3:
sw $a0, 0($sp)
addi $sp, $sp, -4

li $t0,3
addi $sp, $sp, 4
loop_wait:
lw $a0, ($sp)
li $v0, 21 #wait   # call wait
syscall
blez $a1, loop_wait # return to loop not equal to one 
addi $t0,-1
addi $sp, $sp, 4
bne $t0,$zero,loop_wait
loop_end:
li $v0 10 #prgoram finished call terminate
syscall

