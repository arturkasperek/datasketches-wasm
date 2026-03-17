const path = require("node:path");

const modulePath = process.argv[2]
  ? path.resolve(process.argv[2])
  : path.resolve(__dirname, "../build/wasm-node/wasm/datasketches_hll.js");

async function main() {
  const createModule = require(modulePath);
  const ds = await createModule();

  const sketch = new ds.HllSketch(12, ds.TargetHllType.HLL_8, false);
  for (let i = 0; i < 10000; i += 1) {
    sketch.updateUInt32(i);
  }
  for (let i = 0; i < 2500; i += 1) {
    sketch.updateUInt32(i);
  }

  const serialized = sketch.serializeCompact(0);
  const roundTrip = ds.HllSketch.deserialize(serialized);

  const union = new ds.HllUnion(12);
  union.updateSketch(roundTrip);
  union.updateUInt32(10000);
  union.updateUInt32(10001);

  const result = union.getResult(ds.TargetHllType.HLL_8);

  console.log(`HLL estimate: ${sketch.getEstimate().toFixed(2)}`);
  console.log(`Round-trip estimate: ${roundTrip.getEstimate().toFixed(2)}`);
  console.log(`Union estimate: ${result.getEstimate().toFixed(2)}`);
  console.log(
    `95% interval: [${result.getLowerBound(2).toFixed(2)}, ${result.getUpperBound(2).toFixed(2)}]`,
  );
  console.log(`Serialized bytes: ${serialized.length}`);
}

main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
