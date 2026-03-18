"use strict";

/**
 * Returns the absolute path to datasketches_hll.wasm.
 * Use this when you need to pass locateFile explicitly.
 */
const path = require("path");

function getWasmPath() {
  const pkgDir = path.dirname(require.resolve("./package.json"));
  return path.join(pkgDir, "datasketches_hll.wasm");
}

module.exports = { getWasmPath };
