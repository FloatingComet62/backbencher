{ pkgs, lib, config, inputs, ... }:

{
  packages = with pkgs; [ git gcc gnumake cmake ];
  enterShell = ''
    git --version # Use packages
  '';
}
