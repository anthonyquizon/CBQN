#include "core.h"
#include "builtins.h"
#include "vm/vm.h"
#include "core/ns.h"

const char* repl_readline(void); // from main.c
STATIC_GLOBAL B dbg_handler;

static void dbg_onThrow(void) {
  B msg = thrownMsg;
  c1(dbg_handler, msg);
}

B readline_c1(B t, B x) {
  dec(x);
  const char* ln = repl_readline();
  return ln? utf8Decode0(ln) : m_c32(0);
}

B dbqn_c1(B t, B x) {
  return x;
}

B trap_c1(Md2D* d, B x) { 
  dec(dbg_handler);
  dbg_handler=inc(d->g);
  vm_onThrow=dbg_onThrow;
  return c1(d->f,x);
}

B trap_c2(Md2D* d, B w, B x) { 
  dec(dbg_handler);
  dbg_handler=inc(d->g);
  vm_onThrow=dbg_onThrow;
  return c2(d->f,w,x);
}

//TODO breakpoint
//TODO print

STATIC_GLOBAL B debuggerNS;
B getDebuggerNS(void) {
  if (debuggerNS.u == 0) {
    gc_add_ref(&dbg_handler);
    #define F(X) incG(bi_##X),
    Body* d    = m_nnsDesc("readline", "bqn", "trap");
    debuggerNS = m_nns(d, F(readline)F(dbqn)F(trap));
    #undef F
    gc_add(debuggerNS);
  }
  return incG(debuggerNS);
}
