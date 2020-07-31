.data
arr: .word 10, 7, 8, 9, 1, 5
n: .word 6
aranan: .word 8
lbl4: .asciiz "\n"
.text
main:
addi $sp, $sp, -8
sw $ra, 0($sp)
li $t0,0
sw $t0, 4($sp)
li $t0,0
sw $t0, 4($sp)
lbl3:
lw $t0, 4($sp)
lw $t1,n
bge $t0, $t1, lbl1
la $t0, arr
lw $t1, 4($sp)
add $t1, $t1, $t1
add $t1, $t1, $t1
add $t1, $t0, $t1
lw $t1, ($t1)
lw $t0,aranan
bne $t1, $t0, lbl2
lw $t0, 4($sp)
add $a0 $zero, $t0
jal print_int
j lbl1
lbl2:
lw $t0, 4($sp)
addi $t0, $t0, 1
sw $t0, 4($sp)
lw $t0, 4($sp)
j lbl3
lbl1:
li $v0, 0
lw $ra, 0($sp)
addi $sp, $sp, 8
lw $ra, 0($sp)
addi $sp, $sp, 8
la $a0, lbl4
li $v0, 4
syscall
li $v0 10 #prgoram finished call terminate
syscall

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

read_char:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $v0 12
syscall
lw $ra, 0($sp)
addi $sp, $sp, 4
jr $ra

read_int:
addi $sp, $sp, -4
sw $ra, 0($sp)
li $v0 5
syscall
lw $ra, 0($sp)
addi $sp, $sp, 4
jr $ra

