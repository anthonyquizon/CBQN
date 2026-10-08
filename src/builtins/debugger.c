#include "core.h"
#include "core/ns.h"
#include "vm/vm.h"
#include "vm/load.h"
#include "utils/nfns.h"
#include "builtins.h"

const char* repl_readline(void); // from main.c

STATIC_GLOBAL B dbg_handler;
STATIC_GLOBAL bool dbg_inHandler=false;
STATIC_GLOBAL Env* dbg_envPause; 

STATIC_GLOBAL Body* ctx_ns;
DEFINE_NFN ctx_bqnDesc;
DEFINE_NFN dbg_compDesc;

B ctx_bqn_c2(B t, B w, B x) {
  if (!dbg_inHandler || !dbg_envPause) { 
    thrM("(debug context).BQN: executed outside of pause handler");
  }

  Scope* sc = c(Scope, nfn_objU(t));

  /*u64 i = o2s(w);*/
  /*u64 n = dbg_envPause-envStart + 1;*/
  /*if (i>=n) thrM("(debug context).BQN: frame index out of range");*/
  /*Env* e=dbg_envPause-i;*/

  Block* block = bqn_comp(x, defaultUnknownState(), def_re, sc, COMP_UNK, false, true);
  ptr_dec(sc->body);
  sc->body = ptr_inc(block->bodies[0]);
  B r = execBlockInplace(block, sc);
  ptr_dec(block);

  return r;
}

B ctx_bqn_c1(B t, B x) {
  return ctx_bqn_c2(t, m_f64(0), x);
}

static void dbg_onPause(B msg) {
  if (q_N(dbg_handler) || dbg_inHandler || COMPS_ACTIVE()) { return; }

  inc(msg);
  dbg_envPause=envCurr;
  dbg_inHandler=true;

  if(CATCH) {
    dbg_envPause=NULL;
    dbg_inHandler=false;
    dec(msg);
    rethrow();
  }

  //TODO create frame stack

  Block* initBlock = bqn_comp(m_c8vec_0("\"(REPL initializer)\""), defaultUnknownState(), def_re, NULL, COMP_UNK, false, false);
  Scope* sc = m_scope(initBlock->bodies[0], dbg_envPause->sc, 0, 0, NULL);
  B scVal = tag(sc,OBJ_TAG);
  ptr_dec(initBlock);

  B ns = m_nns(ctx_ns, msg, m_nfn(ctx_bqnDesc, scVal));
  B r=c1(dbg_handler, ns); 
  /*dec(msg);*/
  dec(r);
  ptr_dec(sc);

  dbg_envPause=NULL;
  dbg_inHandler=false;

  popCatch(); 
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

B dbg_comp_c2(B t, B w, B x) { 
  B r = c2(nfn_objU(t), w, x);                // ⟨bytecode, objects, blocks, bodies, …⟩
  HArr* rh = (HArr*)cpyHArr(r);               // fresh copy of the result list; consumes r
  I32Arr* bc = (I32Arr*)cpyI32Arr(rh->a[0]);  // fresh i32 copy of the bytecode; consumes the old element
  rh->a[0] = taga(bc);                        // slot now owns the new array
                                              
  u32* p = (u32*)bc->a;
  u32* e = p + PIA(bc);
  for (; p<e; p = nextBC(p)) if (*p==VARU) *p = VARO;

  return taga(rh);
}

B dbqn_c2(Md1D* d, B w, B x) { 
  vfyStr(x, "•debug.BQN", "𝕩");
  //TODO 
  return x;
}

B dbqn_c1(Md1D* d, B x) { 
  vfyStr(x, "•debug.BQN", "𝕩");

  if (q_N(dbg_handler) || dbg_inHandler || COMPS_ACTIVE()) { 
    thrM("debug.BQN: nested debug calls");
  }

  dbg_handler = d->f;
  inc(dbg_handler);

  if (CATCH) {
    vm_onThrow=NULL;
    dec(dbg_handler);
    dbg_handler=bi_N;
    rethrow();
  }

  vm_onThrow=dbg_onPause;

  HArr_p re = m_harr0v(re_max);
  init_comp(re.a, harr_ptr(def_re), bi_N, bi_N, bi_N);
  re.a[re_compFn] = m_nfn(dbg_compDesc, re.a[re_compFn]);
  B r = rebqn_exec(x, defaultUnknownState(), re.b);

  dec(re.b);
  dec(dbg_handler);
  dbg_handler=bi_N;
  popCatch(); 
  return r;
}

STATIC_GLOBAL B debuggerNS;
B getDebuggerNS(void) {
  if (debuggerNS.u == 0) {
    gc_add_ref(&dbg_handler);

    ctx_bqnDesc = registerNFn(m_c8vec_0("(debug context).Bqn"), ctx_bqn_c1, ctx_bqn_c2);
    ctx_ns = m_nnsDesc("msg","bqn");
    dbg_compDesc = registerNFn(m_c8vec_0("(debug compiler transform)"), c1_bad, dbg_comp_c2);

    #define F(X) incG(bi_##X),
    Body* d    = m_nnsDesc("readline", "bqn", "break");
    debuggerNS = m_nns(d, F(readline)F(dbqn)F(dbreak));
    #undef F
    gc_add(debuggerNS);
  }
  return incG(debuggerNS);
}
