# End-to-end tests for tools/dbg.bqn: pipe commands into the debugger
# and check its output. Run from the repo root: ./bqn tools/test/dbg.bqn
dir←•file.Parent •path                                                      # tools/
bqn←(•file.Parent dir)•file.At "bqn"

# Run dbg.bqn on fixture 𝕨 with input lines 𝕩
Run←{f𝕊cmds:                                                                # stdout∾stderr
  in←∾(∾⟜(@+10))¨cmds
  ·‿out‿err←{stdin⇐in}•SH bqn‿(dir•file.At "dbg.bqn")‿(•path•file.At "cases/"∾f∾".bqn")
  out∾err
}
Has←{∨´⥊𝕩⍷𝕨}                                                                # 𝕨 contains substring 𝕩

tests←⟨
  "list"           ‿"depth1"‿⟨")l"⟩      ‿⟨"-->  F←{𝕊x:"⟩
  "frames depth 1" ‿"depth1"‿⟨")f"⟩      ‿⟨"Frame #0"⟩
  "frames depth 2" ‿"depth2"‿⟨")f"⟩      ‿⟨"Frame #0","Frame #1"⟩
  "frames depth 4" ‿"depth4"‿⟨")f"⟩      ‿("Frame #"⊸∾¨•Fmt¨↕5)
  "up then list"   ‿"depth2"‿⟨")up",")l"⟩‿⟨"-->  F←{𝕊x: "⟩
  "up clamps"      ‿"depth1"‿⟨")up",")up"⟩‿⟨⟩
  "eval in frame"  ‿"depth1"‿⟨"⟨x+10⟩"⟩  ‿⟨"⟨ 10 ⟩"⟩
  # special names; results wrapped in ⟨⟩ so they can't match the source listing
  "𝕩"              ‿"special"‿⟨"⟨𝕩⟩"⟩      ‿⟨"⟨ 10 ⟩"⟩
  "𝕨"              ‿"special"‿⟨"⟨𝕨⟩"⟩      ‿⟨"⟨ 2 ⟩"⟩
  "𝕊"              ‿"special"‿⟨"⟨𝕊≡G⟩"⟩    ‿⟨"⟨ 1 ⟩"⟩
  "𝕨 monadic"      ‿"depth1" ‿⟨"⟨𝕨⊣5⟩"⟩    ‿⟨"⟨ 5 ⟩"⟩
  "𝕩 after up"     ‿"special"‿⟨")up","⟨𝕩⟩"⟩‿⟨"⟨ 1 ⟩"⟩
  "𝕊 after up"     ‿"special"‿⟨")up","⟨𝕊≡F⟩"⟩‿⟨"⟨ 1 ⟩"⟩
⟩

r←{name‿f‿cmds‿want:
  out←f Run cmds
  bad←(¬out⊸Has¨want)/want
  bad∾↩(out Has "Error:")/⟨"(no Error: in output)"⟩
  ok←0=≠bad
  •Out (ok⊑"FAIL "‿"ok   ")∾name
  {𝕊: •Out¨"    missing: "⊸∾¨bad ⋄ •Out "    output:" ⋄ •Out out}⍟(¬ok)@
  ok
}¨tests
•Out (•Fmt +´r)∾"/"∾(•Fmt ≠r)∾" passed"

