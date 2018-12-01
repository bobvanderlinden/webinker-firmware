{ pkgs ? import <nixpkgs> {} }:
with pkgs;
stdenv.mkDerivation rec {
  name = "webinker-firmware";
  buildInputs = [ pkgs.platformio ];
}