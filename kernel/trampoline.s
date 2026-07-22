.segment "TRAMPOLINE"

; Assembly routine that installs the page table, restores context, and executes RTI
; void return_from_trap(uint8_t* page_table);

.global _return_from_trap
_return_from_trap:
    rts

; Copy the process's CPU context into the Trap Segment "Life Raft"
; void copy_to_life_raft(Context* ctx);

.global _copy_to_life_raft
_copy_to_life_raft:
    rts