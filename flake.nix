{
  description = "x86_64-elf OSDEV environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";

    pkgs = import nixpkgs {
      inherit system;
    };

    cross-embedded = pkgs.pkgsCross.x86_64-embedded;
  in {
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        xorriso
        qemu
        zsh
        ninja

        gcc
        glibc.static

        # freestanding kernel toolchain
        cross-embedded.buildPackages.gcc
        cross-embedded.buildPackages.binutils
      ];
      shellHook = ''
                  exec zsh
                '';
    };
  };
}
