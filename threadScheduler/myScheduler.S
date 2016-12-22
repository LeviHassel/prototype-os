.section .exceptions.exit.user, "xa"

call getFlag
beq r2, r0, not_set
mov r4, sp
call mythread_scheduler
#stw fp, -4(sp)
mov sp, r2			#need to verify that correct stack pointer is loaded
#ldw fp, -4(sp)
call resetFlag

not_set:
