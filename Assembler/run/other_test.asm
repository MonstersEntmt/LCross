bits 16

var: dd 5.0 ; 00 00 A0 40

fadd dword[var] ; D8 06 00 00
fadd st0, st0   ; DC C0
fcmovb st0, st0