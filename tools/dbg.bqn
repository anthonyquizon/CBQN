#!/usr/bin/env bqn
esc←@+27
win_n←5 # size of src window
cont‿loop←↕2

•Out "Running "∾⊑•args

List←{ctx𝕊f:
  ⟨src,file,s,e⟩←f⊑ctx.frames

  •Out file
  l←+´s↑src=@+10 # line number
  n←≠p←src⊔˜+`»src=@+10 
  
  # TODO relative line number
  i←(⊣/˜n⊸>∧0⊸<)(l-(⌊win_n÷2))+↕win_n 
  sufx←(l=i)⊏"     "‿" --> "
  cc←i (•Fmt∘⊣∾⊢)¨sufx

  of←∾(¯1¨¨cc)∾¨1+i⊏(↕≠src)⊔˜+`src=@+10 # offset numbers
  fr←∾cc∾¨i⊏p
  fr↩∾{(esc∾"[4m")‿(esc∾"[24m")∾¨⋈¨𝕩}⌾((of⊐s‿e)⊸⊏) fr
  •Out fr
  loop
}

Exit    ←{𝕊: •Out "Exit" ⋄ •Exit 0 }
Continue←{𝕊:•Out "Continue" ⋄ cont}
Step    ←{· 𝕊 ·:•Out "TODO: Step" ⋄ loop}

{𝕊 ctx:
  i←0 ⋄ ctx List i # i: frame index in stack
  {𝕊:
    {
      @: Exit @
    ; ")up": ctx List i↩(1+i)⌊≠ctx.frames⋄ 1
    ; ")down": ctx List i↩(1+i)⌈0⋄ 1
    ; ")l": ctx List i
    ; ")list": ctx List i
    ; ")c": Continue @
    ; ")continue": Continue @
    # TODO step
    ; ")s": ctx Step i
    ; ")step": ctx Step i
    ; {•Show i ctx.BQN 𝕩}⎊{𝕊:•Out∘•CurrentError@}𝕩 ⋄ loop
    } •Readline@ # TODO trim? TODO autocomplete
  } •_while_ ⊢ 1
} •debug._BQN "•Import """ ∾ (⊑•args) ∾ """"
