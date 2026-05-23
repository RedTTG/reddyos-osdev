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

    cross = pkgs.pkgsCross.x86_64-embedded;
  in {
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        xorriso
        qemu
        zsh
        clang

        gcc
        glibc.static
        # freestanding kernel toolchain
        cross.buildPackages.gcc
        cross.buildPackages.binutils
      ];
      shellHook = ''
                  exec zsh
                '';
    };
  };
}
