
.data
g_var: .word 1

.text
main:
 addiu $sp,$sp,-4 #push return address
 sw $ra,($sp)
 addiu $sp,$sp,-4 #push frame pointer
 sw $fp,($sp)
 addiu $fp,$sp,8 #formals space
 addiu $sp,$sp,-32 #locals space
 #assignment
 j label0002
.data
label0001: .asciiz "Please Enter Your Name:"
.text
label0002:
 la $v1,label0001
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #read variable
 li $a0,1000
 li $v0,9
 syscall
 sw $v0,-16($fp) #szName
 lw $a0,-16($fp) #szName
 li $a1,1000
 li $v0,8
 syscall
 #assignment
 j label0004
.data
label0003: .asciiz "Please Enter you Address:"
.text
label0004:
 la $v1,label0003
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #read variable
 li $a0,1000
 li $v0,9
 syscall
 sw $v0,-20($fp) #szAddr
 lw $a0,-20($fp) #szAddr
 li $a1,1000
 li $v0,8
 syscall
 #assignment
 j label0006
.data
label0005: .asciiz "Please Enter you Age:"
.text
label0006:
 la $v1,label0005
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #read variable
 li $v0,5
 syscall
 sw $v0,-32($fp) #age
 #assignment
 j label0008
.data
label0007: .asciiz "Please enter you GPA:"
.text
label0008:
 la $v1,label0007
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #read variable
 li $v0,5
 syscall
 sw $v0,-36($fp) #total
 #assignment
 lw $v1,-36($fp) #total
 addiu $sp,$sp,-4
 sw $v1,($sp)
 lw $v1,-36($fp) #total
 lw $t0,($sp)
 addiu $sp,$sp,4
 mul $v1,$t0,$v1
 addiu $sp,$sp,-4
 sw $v1,($sp)
 lw $v1,-36($fp) #total
 addiu $sp,$sp,-4
 sw $v1,($sp)
 lw $v1,-36($fp) #total
 lw $t0,($sp)
 addiu $sp,$sp,4
 div $v1,$t0,$v1
 lw $t0,($sp)
 addiu $sp,$sp,4
 sub $v1,$t0,$v1
 addiu $sp,$sp,-4
 sw $v1,($sp)
 lw $v1,-36($fp) #total
 lw $t0,($sp)
 addiu $sp,$sp,4
 sub $v1,$t0,$v1
 addiu $sp,$sp,-4
 sw $v1,($sp)
 lw $v1,-36($fp) #total
 addiu $sp,$sp,-4
 sw $v1,($sp)
 li $v1,2
 lw $t0,($sp)
 addiu $sp,$sp,4
 div $v1,$t0,$v1
 lw $t0,($sp)
 addiu $sp,$sp,4
 add $v1,$t0,$v1
 sw $v1,-36($fp) #total
 #write variable
 lw $a0,-36($fp) #total
 li $v0,1
 syscall
 #assignment
 j label000A
.data
label0009: .asciiz "Your Name is :"
.text
label000A:
 la $v1,label0009
 move $a1,$v1
 lw $v1,-16($fp) #szName
 move $a2,$v1
 li $a0,1000
 li $v0,9
 syscall
 move $v1,$v0
label000B:
 lb $t0,($a1)
 beqz $t0,label000C
 sb $t0,($v0)
 addiu $a1,$a1,1
 addiu $v0,$v0,1
 j label000B
label000C:
label000D:
 lb $t0,($a2)
 beqz $t0,label000E
 sb $t0,($v0)
 addiu $a2,$a2,1
 addiu $v0,$v0,1
 j label000D
label000E:
 li $t0,0
 sb $t0,($v0)
 move $a1,$v1
 j label0010
.data
label000F: .asciiz "And your Address is :"
.text
label0010:
 la $v1,label000F
 move $a2,$v1
 li $a0,1000
 li $v0,9
 syscall
 move $v1,$v0
label0011:
 lb $t0,($a1)
 beqz $t0,label0012
 sb $t0,($v0)
 addiu $a1,$a1,1
 addiu $v0,$v0,1
 j label0011
label0012:
label0013:
 lb $t0,($a2)
 beqz $t0,label0014
 sb $t0,($v0)
 addiu $a2,$a2,1
 addiu $v0,$v0,1
 j label0013
label0014:
 li $t0,0
 sb $t0,($v0)
 move $a1,$v1
 lw $v1,-20($fp) #szAddr
 move $a2,$v1
 li $a0,1000
 li $v0,9
 syscall
 move $v1,$v0
label0015:
 lb $t0,($a1)
 beqz $t0,label0016
 sb $t0,($v0)
 addiu $a1,$a1,1
 addiu $v0,$v0,1
 j label0015
label0016:
label0017:
 lb $t0,($a2)
 beqz $t0,label0018
 sb $t0,($v0)
 addiu $a2,$a2,1
 addiu $v0,$v0,1
 j label0017
label0018:
 li $t0,0
 sb $t0,($v0)
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #assignment
 j label001A
.data
label0019: .asciiz "Your Age is :"
.text
label001A:
 la $v1,label0019
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #write variable
 lw $a0,-32($fp) #age
 li $v0,1
 syscall
 #assignment
 j label001C
.data
label001B: .asciiz " Your GPA is :"
.text
label001C:
 la $v1,label001B
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #write variable
 lw $a0,-36($fp) #total
 li $v0,1
 syscall
 #assignment
 j label001E
.data
label001D: .asciiz "\nThe length of your name is: "
.text
label001E:
 la $v1,label001D
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #write variable
 lw $a0,-40($fp) #len
 li $v0,1
 syscall
 #assignment
 j label0020
.data
label001F: .asciiz " character"
.text
label0020:
 la $v1,label001F
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #assignment
 j label0022
.data
label0021: .asciiz "deja"
.text
label0022:
 la $v1,label0021
 sw $v1,-24($fp) #szFirst
 #assignment
 j label0024
.data
label0023: .asciiz "Vu"
.text
label0024:
 la $v1,label0023
 sw $v1,-28($fp) #szSecond
 #assignment
 j label0026
.data
label0025: .asciiz "\n"
.text
label0026:
 la $v1,label0025
 move $a1,$v1
 lw $v1,-24($fp) #szFirst
 move $a2,$v1
 li $a0,1000
 li $v0,9
 syscall
 move $v1,$v0
label0027:
 lb $t0,($a1)
 beqz $t0,label0028
 sb $t0,($v0)
 addiu $a1,$a1,1
 addiu $v0,$v0,1
 j label0027
label0028:
label0029:
 lb $t0,($a2)
 beqz $t0,label002A
 sb $t0,($v0)
 addiu $a2,$a2,1
 addiu $v0,$v0,1
 j label0029
label002A:
 li $t0,0
 sb $t0,($v0)
 move $a1,$v1
 lw $v1,-28($fp) #szSecond
 move $a2,$v1
 li $a0,1000
 li $v0,9
 syscall
 move $v1,$v0
label002B:
 lb $t0,($a1)
 beqz $t0,label002C
 sb $t0,($v0)
 addiu $a1,$a1,1
 addiu $v0,$v0,1
 j label002B
label002C:
label002D:
 lb $t0,($a2)
 beqz $t0,label002E
 sb $t0,($v0)
 addiu $a2,$a2,1
 addiu $v0,$v0,1
 j label002D
label002E:
 li $t0,0
 sb $t0,($v0)
 move $a1,$v1
 j label0030
.data
label002F: .asciiz "\n"
.text
label0030:
 la $v1,label002F
 move $a2,$v1
 li $a0,1000
 li $v0,9
 syscall
 move $v1,$v0
label0031:
 lb $t0,($a1)
 beqz $t0,label0032
 sb $t0,($v0)
 addiu $a1,$a1,1
 addiu $v0,$v0,1
 j label0031
label0032:
label0033:
 lb $t0,($a2)
 beqz $t0,label0034
 sb $t0,($v0)
 addiu $a2,$a2,1
 addiu $v0,$v0,1
 j label0033
label0034:
 li $t0,0
 sb $t0,($v0)
 sw $v1,-12($fp) #szMsg
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #write variable
 lw $a0,-12($fp) #szMsg
 li $v0,4
 syscall
 #return statement
 li $v1,0
 move $sp,$fp
 lw $t0,-4($fp)
 lw $fp,-8($fp)
 jr $t0
 move $sp,$fp #clean up
 lw $t0,-4($fp)
 lw $fp,-8($fp)
 jr $t0

