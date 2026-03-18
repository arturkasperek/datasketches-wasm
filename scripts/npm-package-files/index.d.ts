export type {
  DataSketchesHllModule,
  DataSketchesHllModuleOptions,
  HllSketch,
  HllSketchConstructor,
  HllUnion,
  HllUnionConstructor,
} from "./datasketches_hll";
export { TargetHllType } from "./datasketches_hll";

import type { DataSketchesHllModule, DataSketchesHllModuleOptions } from "./datasketches_hll";

declare function createModule(
  options?: DataSketchesHllModuleOptions
): Promise<DataSketchesHllModule>;

export default createModule;
