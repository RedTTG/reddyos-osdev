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
  in {
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        nasm
        xorriso
        grub2
        qemu

        # cross compiler
        pkgsCross.x86_64-embedded.buildPackages.gcc
        pkgsCross.x86_64-embedded.buildPackages.binutils
      ];

      shellHook = ''
        echo "OSDEV environment ready"
      '';
    };
  };
}