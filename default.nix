{ stdenv, cmake, lib }:

stdenv.mkDerivation {
  pname = "varbor";
  version = "1.0.0";

  src = ./.;

  nativeBuildInputs = [ cmake ];

  cmakeFlags = [
    "-DBUILD_TESTING=OFF"
  ];

  meta = {
    description = "variant cbor — header-only C++ CBOR library";
    platforms = lib.platforms.all;
  };
}
