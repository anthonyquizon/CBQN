#!/usr/bin/env bqn
esc←@+27
n←5                                                        # size of src window
u←esc∾¨"[4m"‿"[24m"                                       # highlight ansi codes 
cont‿loop←↕2

•Out "Running "∾⊑•args

List←{ctx𝕊f:
  ⟨src,file,s,e⟩←f⊑ctx.frames
  •Out file
  l←+´s↑nl←src=@+10 ⋄ g←⊔+`»nl                            # mark new lines, get target line number. group indices by lines
  w←g⊏˜i←(0⌈l-n-1)↓↕(≠g)⌊l+n                              # line window ie. n-2 n-1 n n+1 n+2
  x←7↑¨(•Fmt¨1+i)∾¨(l=i)⊏" "‿" --> "                      # line prefi*x* eg. "1 --> "
  o←∾(¯1¨¨x)∾¨1+w                                         # offset lines. Mark prefix with ¯1 to avoid s‿e matches
  ls↩∾{u∾¨𝕩}⌾((o⊐s‿e)⊸⊏)⋈¨∾x∾¨⊏⟜src¨w                     # join prefi*x* and windowed src lines
  •Out fr
  loop
}

Frames←{ctx𝕊i:
  fr←(↕≠ctx.frames) {i𝕊⟨src,file,s,e⟩:
    •SHOW s
    •SHOW src
    •Fmt i∾file
    
  }¨ctx.frames

  # TODO
  loop
}

Exit    ←{ 𝕊 :•Out "Exit"       ⋄ •Exit 0 }
Continue←{ 𝕊 :•Out "Continue"   ⋄ cont    }
Step    ←{·𝕊·:•Out "TODO: Step" ⋄ loop    }

{𝕊 ctx:
  i←0 ⋄ ctx List i                                        # i: frame index in stack
  {𝕊:
    {
      @: Exit @
    ; ")up": ctx List i↩(i+1)⌊¯1+≠ctx.frames ⋄ loop
    ; ")down": ctx List i↩(i-1)⌈0 ⋄ loop
    ; ")f": ctx Frames i
    ; ")frames": ctx Frames i
    ; ")l": ctx List i
    ; ")list": ctx List i
    ; ")c": Continue @
    ; ")continue": Continue @
    # TODO step
    ; ")s": ctx Step i
    ; ")step": ctx Step i
    ; {•Show i ctx.BQN 𝕩}⎊{𝕊:•Out∘•CurrentError@}𝕩 ⋄ loop
    } •Readline@                                          # TODO trim? TODO autocomplete
  } •_while_ ⊢ 1
} •debug._BQN "•Import """ ∾ (⊑•args) ∾ """"

