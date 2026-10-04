#include "core.h"
#include "core/ns.h"
#include "vm/vm.h"
#include "vm/load.h"
#include "utils/nfns.h"
#include "builtins.h"

const char* repl_readline(void); // from main.c
STATIC_GLOBAL B dbg_handler;
STATIC_GLOBAL bool dbg_inHandler=false;
STATIC_GLOBAL Env* dbg_envTop; 

STATIC_GLOBAL Body* ctx_ns;
DEFINE_NFN ctx_bqnDesc;

B ctx_bqn_c2(B t, B w, B x) {
  //TODO run ctx.bqn with frame
  return x;
}

B ctx_bqn_c1(B t, B x) {
  Block* initBlock = bqn_comp(m_c8vec_0("\"(Ctx BQN)\""), defaultUnknownState(), def_re, NULL, COMP_UNK, false, false);
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

static NOINLINE void ctx_init() {
  ctx_ns = m_nnsDesc("msg", "bqn");
  ctx_bqnDesc = registerNFn(m_c8vec_0("(debug context).Bqn"), ctx_bqn_c1, ctx_bqn_c2);
}

static void dbg_onPause(B kind, B msg) {
  if (q_N(dbg_handler) || dbg_inHandler || COMPS_ACTIVE()) { return; }

  inc(msg);
  dbg_envTop=envCurr;
  dbg_inHandler=true;

  if (ctx_ns==NULL) ctx_init();

  B ns = m_nns(ctx_ns, msg, m_nfn(ctx_bqnDesc, bi_N));
  B r=c1(dbg_handler, ns); 
  dec(msg);
  dec(r);
  dbg_envTop=NULL;
  dbg_inHandler=false;
}

B readline_c1(B t, B x) {
  dec(x);
  const char* ln = repl_readline();
  return ln? utf8Decode0(ln) : m_c32(0);
}

B dbreak_c1(B t, B x) {
  dbg_onPause(m_c8vec_0("Breakpoint")); 
  return x;
}

B dbqn_c2(Md1D* d, B w, B x) { 
  vfyStr(x, "•BQN", "𝕩");
  //TODO 
  return x;
}

B dbqn_c1(Md1D* d, B x) { 
  vfyStr(x, "•BQN", "𝕩");

  dec(dbg_handler);
  dbg_handler=inc(d->f);
  vm_onThrow=dbg_onPause;

  return rebqn_exec(x, defaultUnknownState(), def_re);
}

STATIC_GLOBAL B debuggerNS;
B getDebuggerNS(void) {
  if (debuggerNS.u == 0) {
    gc_add_ref(&dbg_handler);
    #define F(X) incG(bi_##X),
    Body* d    = m_nnsDesc("readline", "bqn", "break");
    debuggerNS = m_nns(d, F(readline)F(dbqn)F(dbreak));
    #undef F
    gc_add(debuggerNS);
  }
  return incG(debuggerNS);
}
