.section .eh_frame
.globl __EH_FRAME_BEGIN__
__EH_FRAME_BEGIN__:

.section .eh_frame_end
.globl __EH_FRAME_END__
__EH_FRAME_END__:
        .long 0     // see gcc/crtstuff.c __FRAME_END__