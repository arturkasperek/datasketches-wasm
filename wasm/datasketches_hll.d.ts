export enum TargetHllType {
  HLL_4 = 0,
  HLL_6 = 1,
  HLL_8 = 2,
}

export interface DataSketchesHllModuleOptions {
  locateFile?: (path: string, scriptDirectory: string) => string;
  onRuntimeInitialized?: () => void;
  noExitRuntime?: boolean;
  print?: (...args: unknown[]) => void;
  printErr?: (...args: unknown[]) => void;
  wasmBinary?: ArrayBuffer | Uint8Array;
  arguments?: string[];
  thisProgram?: string;
}

export interface HllSketch {
  reset(): void;
  updateString(value: string): void;
  updateUInt64(value: bigint): void;
  updateUInt32(value: number): void;
  updateInt64(value: bigint): void;
  updateInt32(value: number): void;
  updateDouble(value: number): void;
  updateFloat(value: number): void;
  updateBytes(value: ArrayLike<number>): void;
  getEstimate(): number;
  getCompositeEstimate(): number;
  getLowerBound(numStdDev: number): number;
  getUpperBound(numStdDev: number): number;
  getLgConfigK(): number;
  getTargetType(): TargetHllType;
  isCompact(): boolean;
  isEmpty(): boolean;
  getCompactSerializationBytes(): number;
  getUpdatableSerializationBytes(): number;
  toString(summary?: boolean, detail?: boolean, auxDetail?: boolean, all?: boolean): string;
  serializeCompact(headerSizeBytes?: number): Uint8Array;
  serializeUpdatable(): Uint8Array;
  delete(): void;
}

export interface HllSketchConstructor {
  new (lgConfigK: number, targetType: TargetHllType, startFullSize: boolean): HllSketch;
  deserialize(bytes: ArrayLike<number>): HllSketch;
  getMaxUpdatableSerializationBytes(lgConfigK: number, targetType: TargetHllType): number;
  getRelErr(upperBound: boolean, unioned: boolean, lgConfigK: number, numStdDev: number): number;
}

export interface HllUnion {
  getEstimate(): number;
  getCompositeEstimate(): number;
  getLowerBound(numStdDev: number): number;
  getUpperBound(numStdDev: number): number;
  getLgConfigK(): number;
  getTargetType(): TargetHllType;
  isEmpty(): boolean;
  reset(): void;
  getResult(targetType: TargetHllType): HllSketch;
  updateSketch(sketch: HllSketch): void;
  updateString(value: string): void;
  updateUInt64(value: bigint): void;
  updateUInt32(value: number): void;
  updateInt64(value: bigint): void;
  updateInt32(value: number): void;
  updateDouble(value: number): void;
  updateFloat(value: number): void;
  updateBytes(value: ArrayLike<number>): void;
  delete(): void;
}

export interface HllUnionConstructor {
  new (lgMaxK: number): HllUnion;
  getRelErr(upperBound: boolean, unioned: boolean, lgConfigK: number, numStdDev: number): number;
}

export interface DataSketchesHllModule {
  TargetHllType: typeof TargetHllType;
  HllSketch: HllSketchConstructor;
  HllUnion: HllUnionConstructor;
}

declare function DataSketchesHllModule(
  options?: DataSketchesHllModuleOptions
): Promise<DataSketchesHllModule>;

export = DataSketchesHllModule;
