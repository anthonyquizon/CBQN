#include "core.h"
#include "core/ns.h"
#include "vm/vm.h"
#include "vm/load.h"
#include "builtins.h"

const char* repl_readline(void); // from main.c
STATIC_GLOBAL B dbg_handler;
STATIC_GLOBAL bool dbg_inHandler=false;
STATIC_GLOBAL Env* dbg_envTop;

static void dbg_onThrow(void) {
  if (COMPS_ACTIVE()) { return; }
  if (dbg_inHandler) { return; }

  B msg = thrownMsg;
  inc(msg);

  dbg_envTop=envCurr;
  dbg_inHandler=true;

  B res=c1(dbg_handler, msg); //TODO create namespace with number of frames
  dec(res);
  dbg_envTop=NULL;
  dbg_inHandler=false;
}

B readline_c1(B t, B x) {
  dec(x);
  const char* ln = repl_readline();
  return ln? utf8Decode0(ln) : m_c32(0);
}

// 𝕨: frame number to use from stack
B framebqn_c2(B t, B w, B x) {
  Block* initBlock = bqn_comp(m_c8vec_0("\"(Debugger initializer)\""), defaultUnknownState(), def_re, NULL, COMP_UNK, false, false);
  Scope* sc = m_scope(initBlock->bodies[0], dbg_envTop->sc, 0, 0, NULL);
  ptr_dec(initBlock);

  Block* block = bqn_comp(x, defaultUnknownState(), def_re, sc, COMP_UNK, false, true);
  ptr_dec(sc->body);
  sc->body = ptr_inc(block->bodies[0]);
  B r = execBlockInplace(block, sc);
  ptr_dec(block);
  ptr_dec(sc);

  return r;
}

// defaults to top most frame
B framebqn_c1(B t, B x) {
  return framebqn_c2(t, m_i32(0), x);
}

//TODO setbreak

B trap_c1(Md2D* d, B x) { 
  dec(dbg_handler);
  dbg_handler=inc(d->g);
  vm_onThrow=dbg_onThrow;
  B r=c1(d->f,x);
  return r;
}

B trap_c2(Md2D* d, B w, B x) { 
  dec(dbg_handler);
  dbg_handler=inc(d->g);
  vm_onThrow=dbg_onThrow;
  B r = c2(d->f,w,x);
  return r;
}

//TODO breakpoint
//TODO add constructor that takes config and returns a namespace of functions a la •rand

STATIC_GLOBAL B debuggerNS;
B getDebuggerNS(void) {
  if (debuggerNS.u == 0) {
    gc_add_ref(&dbg_handler);
    #define F(X) incG(bi_##X),
    Body* d    = m_nnsDesc("readline","framebqn","trap");
    debuggerNS = m_nns(d, F(readline)F(framebqn)F(trap));
    #undef F
    gc_add(debuggerNS);
  }
  return incG(debuggerNS);
}

/*B makeDebug_c1(Md1D* t, B x) {*/
  /*if (!isArr(x)) thrM("•_makeDebug 𝕩: Argument must be an array");*/
  /*if (rand_ns==NULL) rand_init();*/
  /*B r = m_nns(rand_ns, r_uB(x.u>>32), r_uB(x.u&0xFFFFFFFF), m_nfn(rand_rangeDesc, bi_N), m_nfn(rand_dealDesc, bi_N), m_nfn(rand_subsetDesc, bi_N));*/
  /*Scope* sc = c(NS,r)->sc;*/
  /*for (i32 i = 2; i < 5; i++) nfn_swapObj(sc->vars[i], incG(r));*/
  /*return r;*/
/*}*/

