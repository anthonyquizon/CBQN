with (import <nixpkgs> {});
mkShell {
  buildInputs = [
    entr
    gcc
    libffi
    replxx
  ];
}

