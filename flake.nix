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
    cross-musl = pkgs.pkgsCross.musl64;
  in {
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        xorriso
        qemu
        zsh

	# freestanding kernel toolchain
        cross-embedded.buildPackages.gcc
        cross-embedded.buildPackages.binutils

        # userspace toolchain (musl libc)
        cross-musl.buildPackages.gcc
        cross-musl.buildPackages.binutils
        cross-musl.musl
      ];
      shellHook = ''
                  exec zsh
                '';
    };
  };
}
