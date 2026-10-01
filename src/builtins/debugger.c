#include "core.h"
#include "builtins.h"
#include "vm/vm.h"
#include "core/ns.h"

const char* repl_readline(void); // from main.c

static void dbg_onPause(void) {
  printf("debugger on throw!\n");
}

B readline_c1(B t, B x) {
  dec(x);
  const char* ln = repl_readline();
  return ln? utf8Decode0(ln) : m_c32(0);
}

B dbqn_c1(B t, B x) {
  return x;
}

B onpause_c1(Md1D* d, B x) { //B f = d->f;
  vm_onThrow=dbg_onPause;
  /*dec(c1(f, x));*/
  return x;
}

//TODO breakpoint
//TODO print

STATIC_GLOBAL B debuggerNS;
B getDebuggerNS(void) {
  if (debuggerNS.u == 0) {
    #define F(X) incG(bi_##X),
    Body* d    = m_nnsDesc("readline", "bqn", "onpause");
    debuggerNS = m_nns(d, F(readline)F(dbqn)F(onpause));
    #undef F
    gc_add(debuggerNS);
  }
  return incG(debuggerNS);
}
