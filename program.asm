;this is a comment
JMP 4


HLT
NOP
MOV R1, R2 ; hellooo?
;MOV R3, R2 ; should jump here, R1 should be untouched
CMP R1, R2
JEQ R1, 2
ADD R1, R2
HLT

