import { Address } from "./address"

export class Function {
    private start: Address
    private end: Address

    constructor(start: Address, end: Address) {
        this.start = start
        this.end = end
    }

    static fromAddress(start: Address): Function {
        return new Function(start, new Address(0))
    }

    toString(): string {
        return `<Function at 0x${this.start.address.toString(16)}/0x${this.end.sub(this.start).address.toString(16)}>`
    }
}