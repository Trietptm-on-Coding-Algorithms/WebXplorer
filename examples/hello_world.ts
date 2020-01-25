@external("", "msg")
export declare function msg(message: ArrayBuffer): void

@external("", "get_next_func")
export declare function getNextFunc(ea: i64): i64

@external("", "auto_wait")
export declare function autoWait(): bool

namespace console {
    function log(message: String): void {
        msg(String.UTF8.encode(message, true))
    }
}

export function main(): bool {
    console.log("Hello, world")

    if (!autoWait()) {
        console.log("Error waiting for analysis, exiting")
        return false
    }

    let addr: i64 = 0
    for (addr = getNextFunc(addr); addr != -1; addr = getNextFunc(addr)) {
        console.log("Addr " + addr.toString());
    }
    return true
}
