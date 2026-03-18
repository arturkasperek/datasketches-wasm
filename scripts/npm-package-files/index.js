"use strict";

/**
 * Wrapper that auto-injects locateFile when not provided.
 * Fixes WASM loading in bundlers (Next.js, etc.) where __dirname resolves incorrectly.
 */
const path = require("path");
const createModule = require("./datasketches_hll.js");

module.exports = function (opts) {
  opts = opts || {};
  if (!opts.locateFile) {
    const pkgDir = path.dirname(require.resolve("./package.json"));
    opts = Object.assign({}, opts, {
      locateFile: (file) => path.join(pkgDir, file),
    });
  }
  return createModule(opts);
};

module.exports.default = module.exports;
