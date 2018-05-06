.text
.global on_timer_intr
on_timer_intr:
    call on_timer_handler
    iret

.global on_disk_intr
on_disk_intr:
    call on_disk_handler
    iret

.global on_ignore_intr
on_ignore_intr:
    call on_ignore_handler
    iret