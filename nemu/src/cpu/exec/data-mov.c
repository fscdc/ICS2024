#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t0);
  operand_write(id_dest, &t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  if (decoding.is_operand_size_16) {
    t0 = reg_w(R_SP);
    t1 = reg_w(R_AX);
    rtl_push(&t1);
    t1 = reg_w(R_CX);
    rtl_push(&t1);
    t1 = reg_w(R_DX);
    rtl_push(&t1);
    t1 = reg_w(R_BX);
    rtl_push(&t1);
    rtl_push(&t0);
    t1 = reg_w(R_BP);
    rtl_push(&t1);
    t1 = reg_w(R_SI);
    rtl_push(&t1);
    t1 = reg_w(R_DI);
    rtl_push(&t1);
  }
  else {
    t0 = reg_l(R_ESP);
    rtl_push(&cpu.eax);
    rtl_push(&cpu.ecx);
    rtl_push(&cpu.edx);
    rtl_push(&cpu.ebx);
    rtl_push(&t0);
    rtl_push(&cpu.ebp);
    rtl_push(&cpu.esi);
    rtl_push(&cpu.edi);
  }
  print_asm("pusha");
}

make_EHelper(popa) {
  if (decoding.is_operand_size_16) {
    rtl_pop(&t0);
    reg_w(R_DI) = t0;
    rtl_pop(&t0);
    reg_w(R_SI) = t0;
    rtl_pop(&t0);
    reg_w(R_BP) = t0;
    rtl_pop(&t0);
    rtl_pop(&t0);
    reg_w(R_BX) = t0;
    rtl_pop(&t0);
    reg_w(R_DX) = t0;
    rtl_pop(&t0);
    reg_w(R_CX) = t0;
    rtl_pop(&t0);
    reg_w(R_AX) = t0;
  }
  else {
    rtl_pop(&cpu.edi);
    rtl_pop(&cpu.esi);
    rtl_pop(&cpu.ebp);
    rtl_pop(&t0);
    rtl_pop(&cpu.ebx);
    rtl_pop(&cpu.edx);
    rtl_pop(&cpu.ecx);
    rtl_pop(&cpu.eax);
  }
  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp, &cpu.ebp);
  if (decoding.is_operand_size_16) {
    rtl_pop(&t0);
    reg_w(R_BP) = t0;
  }
  else {
    rtl_pop(&cpu.ebp);
  }
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    rtl_msb(&t0, &cpu.eax, 2);
    if(t0 == 1) {
      cpu.gpr[R_DX]._16 = (uint16_t)0xffff;
    } else {
      cpu.gpr[R_DX]._16 = 0;
    }
  }
  else {
	  rtl_msb(&t0, &cpu.eax, 4);
    if (t0 == 1) {
      cpu.edx = 0xffffffff;
    } else {
      cpu.edx = 0;
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_shli(&cpu.eax, &cpu.eax, 24);
    rtl_sari(&cpu.eax, &cpu.eax, 16);
    rtl_shri(&cpu.eax, &cpu.eax, 16);
  }
  else {
    rtl_sext(&t0, &cpu.eax, 2);
    cpu.eax = t0;
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
