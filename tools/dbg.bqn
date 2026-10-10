
esc←@+27

SetFrame←{ctx𝕊f:
  ⟨src, file,s,e⟩←f⊑ctx.frames

  # TODO lines
  •Out file
  l←+´s↑src=@+10 # line number
  n←≠p←src⊔˜+`»src=@+10 
  
  # TODO relative line number
  i←(⊣/˜n⊸>∧0⊸<)(l-(⌊v÷2))+↕v 
  sufx←(l=i)⊏"     "‿" --> "
  cc←i (•Fmt∘⊣∾⊢)¨sufx

  of←∾(¯1¨¨cc)∾¨1+i⊏(↕≠src)⊔˜+`src=@+10 # offset numbers
  fr←∾cc∾¨i⊏p
  fr↩∾{(esc∾"[4m")‿(esc∾"[24m")∾¨⋈¨𝕩}⌾((of⊐s‿e)⊸⊏) fr
  •Out fr
}

v←5 # size of src window
{𝕊 ctx:
  f←0 # frame index in stack
  ctx SetFrame f
  {𝕊:
    {
      @: •Out "Continue" ⋄ 0
    ; ")up"  : ctx SetFrame f↩(1+f)⌊≠ctx.frames⋄ 1
    ; ")down"  : ctx SetFrame f↩(1+f)⌈0⋄ 1
    ; 𝕩: •Show∘ctx.BQN⎊{𝕊:•Out∘•CurrentError@}𝕩 ⋄ 1
    } •Readline@ # TODO trim?
  } •_while_ ⊢ 1
} •debug._BQN "
  a←↕b←10

  r←2+2
  q←1+1
  F←{𝕊:
    a←2222
    •Break@
  }

  F@
  #99⊏a
"
