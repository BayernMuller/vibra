export interface VibraModuleOptions {
  locateFile?: (path: string, scriptDirectory: string) => string;
  onRuntimeInitialized?: () => void;
  wasmBinary?: ArrayBuffer | Uint8Array;
}

export interface VibraModule {
  HEAPU8: Uint8Array;
  _malloc(size: number): number;
  _free(ptr: number): void;
  ccall(
    ident: string,
    returnType: string | null,
    argTypes: string[],
    args: Array<number | string>
  ): unknown;
}

export default function createModule(
  options?: VibraModuleOptions
): Promise<VibraModule>;
