#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(or);
}

make_EHelper(sar) {

  // unnecessary to update CF and OF in NEMU
  rtl_sar(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(sar);
}

make_EHelper(shl) {

  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(shl);
}

make_EHelper(shr) {

  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  operand_write(id_dest, &t0);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  rtl_li(&t0, id_dest->val);
  rtl_not(&t0);
  operand_write(id_dest, &t0);
  print_asm_template1(not);
}

make_EHelper(rol) {
  t0 = id_dest->width * 8 - id_src->val;
  rtl_shr(&t1, &id_dest->val, &t0);
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  rtl_or(&t0, &t1, &t2);
  operand_write(id_dest, &t0);
  print_asm_template2(rol);
}
