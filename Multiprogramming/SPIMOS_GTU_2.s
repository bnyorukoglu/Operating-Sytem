
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

li $a0,3 
li $v0,24
syscall # random calll
li $t0,4
mult $a1,$t0
mflo $a1
la $a0, file

add $t1,$a0,$a1;
lw $a0, ($t1)
li $v0, 4
syscall


li $t0,10
start:
li $v0, 20 # call fork 
syscall

bne $a1,$zero,not_child # check wheter child
lw $a0, ($t1)  
li $v0, 19 
syscall  # call execv

j loop_end
not_child:         # WAiting for all children to die
sw $a0, 0($sp)
addi $sp, $sp, -4
addi $t0,-1
bne $t0,$zero,start

li $t0,10
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

