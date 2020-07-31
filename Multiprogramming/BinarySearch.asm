.data
lbl4: .asciiz "Element is at:"
lbl9: .asciiz "Not Found!\n"
lbl10: .asciiz "\n"
array: .word 2, 3, 4, 5, 6, 7, 8, 9, 22, 33, 45
size: .word 11
aranan: .word 22
.text

main:
addi $sp, $sp, -24
sw $ra, 0($sp)
li $t0,0
sw $t0, 4($sp)
lw $t0,size
addi $t0, $t0, -1
sw $t0, 8($sp)
li $t0,0
sw $t0, 12($sp)
lw $t0,aranan
sw $t0, 16($sp)
lbl7:
lw $t0, 4($sp)
lw $t1, 8($sp)
bgt $t0, $t1, lbl1
lw $t0, 4($sp)
lw $t1, 8($sp)
lw $t2, 4($sp)
sub $t3, $t1, $t2
li $t2,2
div $t1, $t3, $t2
add $t2, $t0, $t1
sw $t2, 20($sp)
la $t0, array
lw $t1, 20($sp)
add $t1, $t1, $t1
add $t1, $t1, $t1
add $t1, $t0, $t1
lw $t1, ($t1)
lw $t0, 16($sp)
bne $t1, $t0, lbl3
li $t0,1
sw $t0, 12($sp)
la $a0, lbl4
li $v0, 4
syscall
lw $t0, 20($sp)
add $a0 $zero, $t0
jal print_int
j lbl1
j lbl2
lbl3:
la $t0, array
lw $t1, 20($sp)
add $t1, $t1, $t1
add $t1, $t1, $t1
add $t1, $t0, $t1
lw $t1, ($t1)
lw $t0, 16($sp)
bge $t1, $t0, lbl6
lw $t0, 20($sp)
addi $t0, $t0, 1
sw $t0, 4($sp)
j lbl5
lbl6:
lw $t0, 20($sp)
addi $t0, $t0, -1
sw $t0, 8($sp)
lbl5:
lbl2:
j lbl7
lbl1:
lw $t0, 12($sp)
li $t1,0
bne $t0, $t1, lbl8
la $a0, lbl9
li $v0, 4
syscall
lbl8:
li $v0, 0
lw $ra, 0($sp)
addi $sp, $sp, 24
lw $ra, 0($sp)
addi $sp, $sp, 24
la $a0, lbl10
li $v0, 5
syscall
li $v0 10 #prgoram finished call terminate
syscall

print_int:
addi $sp, $sp, -8
sw $ra, 0($sp)
sw $zero, 4($sp)
sw $a0, 4($sp)
li $v0 1
syscall
lw $ra, 0($sp)
addi $sp, $sp, 8
jr $ra

