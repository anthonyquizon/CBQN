#include "core.h"
#include "builtins.h"
#include "core/ns.h"

const char* repl_readline(void); // from main.c

B readline_c1(B t, B x) {
  dec(x);
  const char* ln = repl_readline();
  return ln? utf8Decode0(ln) : m_c32(0);
}

//TODO breakpoint
//TODO print

STATIC_GLOBAL B debuggerNS;
B getDebuggerNS(void) {
  if (debuggerNS.u == 0) {
    #define F(X) incG(bi_##X),
    Body* d    = m_nnsDesc("readline");
    debuggerNS = m_nns(d, F(readline));
    #undef F
    gc_add(debuggerNS);
  }
  return incG(debuggerNS);
}
