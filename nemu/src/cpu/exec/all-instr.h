#include "cpu/exec.h"

//data-mov.c
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper(leave);


//prefix.c
make_EHelper(operand_size);


//special.c
make_EHelper(nop);
make_EHelper(inv);
make_EHelper(nemu_trap);


// logic.c
make_EHelper(test);
make_EHelper(and);
make_EHelper(xor);
make_EHelper(or);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(setcc);
make_EHelper(not);


// control.c
make_EHelper(jmp);
make_EHelper(jmp_rm);
make_EHelper(jcc);
make_EHelper(call);
make_EHelper(call_rm);
make_EHelper(ret);


// arith.c
make_EHelper(add);
make_EHelper(sub);
make_EHelper(cmp);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(neg);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);


// system.c
make_EHelper(lidt);
make_EHelper(iret);
make_EHelper(int);
make_EHelper(in);
make_EHelper(out);
make_EHelper(mov_cr2r);
make_EHelper(mov_r2cr);