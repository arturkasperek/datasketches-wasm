import { cpSync, mkdirSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import path from "node:path";

function parseArgs(argv) {
  const result = {};
  for (let index = 0; index < argv.length; index += 1) {
    const arg = argv[index];
    if (!arg.startsWith("--")) {
      continue;
    }
    result[arg.slice(2)] = argv[index + 1];
    index += 1;
  }
  return result;
}

const args = parseArgs(process.argv.slice(2));
const moduleDir = path.resolve(args["module-dir"] ?? "");
const outDir = path.resolve(args["out-dir"] ?? "");
const packageName = args["package-name"];
const packageVersion = args["package-version"];
const repositoryUrl = args["repository-url"];
const homepageUrl = args["homepage-url"];

if (!moduleDir || !outDir || !packageName || !packageVersion || !repositoryUrl || !homepageUrl) {
  throw new Error("Expected --module-dir, --out-dir, --package-name, --package-version, --repository-url, and --homepage-url");
}

rmSync(outDir, { recursive: true, force: true });
mkdirSync(outDir, { recursive: true });

for (const fileName of ["datasketches_hll.js", "datasketches_hll.wasm", "datasketches_hll.d.ts"]) {
  cpSync(path.join(moduleDir, fileName), path.join(outDir, fileName));
}

cpSync(path.resolve("LICENSE"), path.join(outDir, "LICENSE"));
cpSync(path.resolve("README.md"), path.join(outDir, "README.md"));

const packageJson = {
  name: packageName,
  version: packageVersion,
  description: "Node.js WebAssembly bindings for Apache DataSketches HLL",
  license: "Apache-2.0",
  repository: {
    type: "git",
    url: repositoryUrl,
  },
  homepage: homepageUrl,
  publishConfig: {
    registry: "https://npm.pkg.github.com",
  },
  main: "datasketches_hll.js",
  types: "datasketches_hll.d.ts",
  files: [
    "datasketches_hll.js",
    "datasketches_hll.wasm",
    "datasketches_hll.d.ts",
    "README.md",
    "LICENSE",
  ],
  keywords: [
    "datasketches",
    "wasm",
    "webassembly",
    "hll",
    "hyperloglog",
    "nodejs",
  ],
  engines: {
    node: ">=18",
  },
  exports: {
    ".": {
      types: "./datasketches_hll.d.ts",
      require: "./datasketches_hll.js",
      default: "./datasketches_hll.js",
    },
  },
};

writeFileSync(path.join(outDir, "package.json"), `${JSON.stringify(packageJson, null, 2)}\n`);

const readmePath = path.join(outDir, "README.md");
const readme = readFileSync(readmePath, "utf8");
const packageReadme = `# ${packageName}

Prebuilt Node.js WebAssembly bindings for Apache DataSketches HLL.

## Usage

\`\`\`js
const createModule = require("${packageName}");

async function main() {
  const ds = await createModule();
  const sketch = new ds.HllSketch(12, ds.TargetHllType.HLL_8, false);

  for (let i = 0; i < 10000; i += 1) {
    sketch.updateUInt32(i);
  }

  console.log(sketch.getEstimate());
}

main();
\`\`\`

## Upstream README

${readme}
`;

writeFileSync(readmePath, packageReadme);
