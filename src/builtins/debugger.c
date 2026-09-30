#include "core.h"
#include "builtins.h"
#include "core/ns.h"

B dreadline_c1(B t, B x) {
    return m_f64(1111);
}

STATIC_GLOBAL B debuggerNS;
B getDebuggerNS(void) {
  if (debuggerNS.u == 0) {
    #define F(X) incG(bi_##X),
    Body* d    = m_nnsDesc( "readline");
    debuggerNS = m_nns(d, F(dreadline));
    #undef F
    gc_add(debuggerNS);
  }
  return incG(debuggerNS);
}
