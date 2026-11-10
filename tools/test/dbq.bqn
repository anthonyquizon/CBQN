dir←•file.Parent •path                                                      # tools/
pr←∨´⟨"-p"⟩⍷•args

F←{name‿f‿cmd‿exp:
  args←⟨(•file.Parent dir)•file.At "bqn", dir•file.At "dbq.bqn", •path•file.At "cases/"∾f∾".bqn"⟩
  stdin←∾∾⟜(@+10)¨cmd
  ·‿out‿err←{stdin⇐stdin} •SH args
  act←out

  •SHOW exp/˜∨´∘⥊∘⍷⟜act¨exp
  #•SHOW exp/˜¬∨´∘⥊∘⍷⟜act¨exp
  
  #•SHOW out
  #bad←(¬out⊸Has¨want)/want
  #bad∾↩((out Has "Error:")∧¬∨´Has⟜"Error:"¨want)/⟨"(no Error: in output)"⟩ # unless the test expects one
  #ok←0=≠bad
  #•Out (ok⊑"FAIL "‿"ok   ")∾name
  #{𝕊: •Out¨"    missing: "⊸∾¨bad ⋄ •Out "    output:" ⋄ •Out out}⍟(pr∨¬ok)@
  #ok
}

F¨⟨
  #"list"             ‿"depth1"‿⟨")l"⟩      ‿⟨"-->  F←{𝕊x:"⟩
  #"frames depth 1"    ‿"depth1"‿⟨")f"⟩      ‿⟨"Frame #0"⟩
  #"frames depth 2"    ‿"depth2"‿⟨")f"⟩      ‿⟨"Frame #0","Frame #1"⟩
  #"frames depth 4"    ‿"depth4"‿⟨")f"⟩      ‿("Frame #"⊸∾¨•Fmt¨↕5)
  "var frame depth 4" ‿"vars"‿⟨"x"⟩      ‿⟨"abcd"⟩
  "var frame depth 4" ‿"vars"‿⟨")up","x"⟩‿⟨"abc"⟩
  "closures"          ‿"closures"‿⟨"a"⟩‿⟨"abc"⟩
  "closures"          ‿"closures"‿⟨"b"⟩‿⟨""⟩
  #"up then list"   ‿"depth2"‿⟨")up",")l"⟩‿⟨"-->  F←{𝕊x: "⟩
  #"up clamps"      ‿"depth1"‿⟨")up",")up"⟩‿⟨⟩
  #"eval in frame"  ‿"depth1"‿⟨"⟨x+10⟩"⟩  ‿⟨"⟨ 10 ⟩"⟩
  ## special names; results wrapped in ⟨⟩ so they can't match the source listing
  #"𝕩"              ‿"special"‿⟨"⟨𝕩⟩"⟩      ‿⟨"⟨ 10 ⟩"⟩
  #"𝕨"              ‿"special"‿⟨"⟨𝕨⟩"⟩      ‿⟨"⟨ 2 ⟩"⟩
  #"𝕊"              ‿"special"‿⟨"⟨𝕊≡G⟩"⟩    ‿⟨"⟨ 1 ⟩"⟩
  #"𝕨 monadic"      ‿"depth1" ‿⟨"⟨𝕨⊣5⟩"⟩    ‿⟨"⟨ 5 ⟩"⟩
  #"𝕩 after up"     ‿"special"‿⟨")up","⟨𝕩⟩"⟩‿⟨"⟨ 1 ⟩"⟩
  #"𝕊 after up"     ‿"special"‿⟨")up","⟨𝕊≡F⟩"⟩‿⟨"⟨ 1 ⟩"⟩
  ## an error inside an eval must not clobber the paused error that continue rethrows
  #"eval error then continue"‿"depth1"‿⟨"!0",")c"⟩‿⟨"Error: Assertion error","depth1.bqn:1:"⟩
⟩
#•Out (•Fmt +´r)∾"/"∾(•Fmt ≠r)∾" passed"

