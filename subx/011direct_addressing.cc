//: operating directly on a register

:(scenario add_r32_to_r32)
% Reg[0].i = 0x10;
% Reg[3].i = 1;
# op  ModR/M  SIB   displacement  immediate
  01  d8                                      # add EBX (reg 3) to EAX (reg 0)
+run: add reg 3 to effective address
+run: effective address is reg 0
+run: storing 0x00000011

:(before "End Single-Byte Opcodes")
case 0x01: {  // add r32 to r/m32
  uint8_t modrm = next();
  uint8_t arg2 = (modrm>>3)&0x7;
  trace(2, "run") << "add reg " << NUM(arg2) << " to effective address" << end();
  int32_t* arg1 = effective_address(modrm);
  BINARY_ARITHMETIC_OP(+, *arg1, Reg[arg2].i);
  break;
}

:(code)
// Implement tables 2-2 and 2-3 in the Intel manual, Volume 2.
// We return a pointer so that instructions can write to multiple bytes in
// 'Mem' at once.
int32_t* effective_address(uint8_t modrm) {
  uint8_t mod = (modrm>>6);
  // ignore middle 3 'reg opcode' bits
  uint8_t rm = modrm & 0x7;
  int32_t* result = 0;
  switch (mod) {
  case 3:
    // mod 3 is just register direct addressing
    trace(2, "run") << "effective address is reg " << NUM(rm) << end();
    result = &Reg[rm].i;
    break;
  // End Mod Special-cases
  default:
    cerr << "unrecognized mod bits: " << NUM(mod) << '\n';
    exit(1);
  }
  return result;
}

//:: subtract

:(scenario subtract_r32_from_r32)
% Reg[0].i = 10;
% Reg[3].i = 1;
# op  ModR/M  SIB   displacement  immediate
  29  d8                                      # subtract EBX (reg 3) from EAX (reg 0)
+run: subtract reg 3 from effective address
+run: effective address is reg 0
+run: storing 0x00000009

:(before "End Single-Byte Opcodes")
case 0x29: {  // subtract r32 from r/m32
  uint8_t modrm = next();
  uint8_t arg2 = (modrm>>3)&0x7;
  trace(2, "run") << "subtract reg " << NUM(arg2) << " from effective address" << end();
  int32_t* arg1 = effective_address(modrm);
  BINARY_ARITHMETIC_OP(-, *arg1, Reg[arg2].i);
  break;
}
