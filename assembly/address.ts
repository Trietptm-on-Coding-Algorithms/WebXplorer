export class Address {
    address: u64

    constructor(address: u64) {
        this.address = address
    }

    add(offset: Address): Address {
        return new Address(this.address + offset.address)
    }

    sub(offset: Address): Address {
        return new Address(this.address - offset.address)
    }

    equals(other: Address): bool {
        return this.address === other.address
    }

    toString(): string {
        return `<Address at 0x${this.address.toString(16)}>`
    }
}