// @ts-ignore
@external("", "msg")
declare function msg(message: ArrayBuffer): void

// @ts-ignore
@external("", "auto_wait")
declare function autoWait(): bool

export namespace console {
  export function log(message: string): void {
    msg(String.UTF8.encode(message, true))
  }
}

import { Address } from "./address"
import { Function } from "./function"

export class BinaryView {
  static CURRENTLY_OPENED: BinaryView = new BinaryView()

  updateAnalysis(): bool {
    return autoWait()
  }

  getFunctions(): Array<Function> {
    return [Function.fromAddress(new Address(0))]
  }
}

export { Address, Function }
