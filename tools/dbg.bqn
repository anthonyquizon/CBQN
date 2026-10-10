{𝕊 ctx:
  •Out ctx.msg

  {𝕊:
    {
      @: 0
    ; 𝕩: •Show∘ctx.BQN⎊{𝕊:•Out∘•CurrentError@}𝕩 ⋄ 1
    } •Readline@
  } •_while_ ⊢ 1
} •debug._BQN "
  a←↕b←10
  •Break@
  99⊏a
"
