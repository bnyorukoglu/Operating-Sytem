# Generated from: Collatz.c
# Generated at: 2020-04-08 17:57:02.350358
.data
lbl5: .asciiz "Enter number: "
.text

main:
addi $sp, $sp, -12
sw $ra, 0($sp)
la $a0, lbl5
li $v0, 4
syscall
jal read_int
sw $v0, 4($sp)
lw $t0, 4($sp)
add $a0 $zero, $t0
jal collatz
sw $v0, 8($sp)
li $v0, 0
lw $ra, 0($sp)
addi $sp, $sp, 12
lw $ra, 0($sp)
addi $sp, $sp, 12
li $v0 10 #prgoram finished call terminate
syscall

getRemainder:
addi $sp, $sp, -12
sw $ra, 0($sp)
sw $zero, 4($sp)
sw $zero, 8($sp)
sw $a0, 4($sp)
sw $a1, 8($sp)
lw $t0, 4($sp)
lw $t1, 8($sp)
lw $t2, 4($sp)
lw $t3, 8($sp)
div $t4, $t2, $t3
mul $t2, $t1, $t4
sub $t1, $t0, $t2
add $v0, $zero, $t1
lw $ra, 0($sp)
addi $sp, $sp, 12
jr $ra
lw $ra, 0($sp)
addi $sp, $sp, 12
jr $ra

collatz:
addi $sp, $sp, -16
sw $ra, 0($sp)
sw $zero, 4($sp)
sw $a0, 4($sp)
li $t0,1
sw $t0, 8($sp)
li $t0,0
sw $t0, 12($sp)
li $t0,0
sw $t0, 12($sp)
lbl4:
lw $t0, 4($sp)
li $t1,1
beq $t0, $t1, lbl1
lw $t0, 4($sp)
add $a0 $zero, $t0
jal print_int
li $a0 32
jal print_char
lw $t0, 4($sp)
add $a0 $zero, $t0
li $a1 2
jal getRemainder
li $t0,0
bne $v0, $t0, lbl3
lw $t0, 4($sp)
li $t2,2
div $t1, $t0, $t2
sw $t1, 4($sp)
j lbl2
lbl3:
lw $t0, 4($sp)
li $t2,3
mul $t1, $t0, $t2
addi $t1, $t1, 1
sw $t1, 4($sp)
lbl2:
lw $t0, 8($sp)
lw $t1, 8($sp)
addi $t1, $t1, 1
sw $t1, 8($sp)
lw $t0, 12($sp)
addi $t0, $t0, 1
sw $t0, 12($sp)
lw $t0, 12($sp)
j lbl4
lbl1:
lw $t0, 4($sp)
add $a0 $zero, $t0
jal print_int
lw $t0, 8($sp)
add $v0, $zero, $t0
lw $ra, 0($sp)
addi $sp, $sp, 16
jr $ra
lw $ra, 0($sp)
addi $sp, $sp, 16
jr $ra

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

print_char:
addi $sp, $sp, -8
sw $ra, 0($sp)
sw $zero, 4($sp)
sw $a0, 4($sp)
li $v0 11
syscall
lw $ra, 0($sp)
addi $sp, $sp, 8
jr $ra

read_int:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $v0 5
syscall
lw $ra, 0($sp)
addi $sp, $sp, 4
jr $ra

