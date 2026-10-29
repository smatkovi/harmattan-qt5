.pragma library

// The calculator's arithmetic: one operation at a time, as the N9's (a
// second operator first works out the one before it).

var MAX_DIGITS = 15

// a result as the line shows it: 12 significant digits (no 0.1 + 0.2
// tail), very large or very small ones with an exponent
function format(n) {
    if (!isFinite(n))
        return ""
    if (n === 0)
        return "0"
    var a = Math.abs(n)
    if (a >= 1e15 || a < 1e-9) {
        var e = n.toExponential(9).split("e")
        var m = e[0].indexOf(".") >= 0 ? e[0].replace(/0+$/, "").replace(/\.$/, "") : e[0]
        return m + "e" + e[1].replace("+", "")
    }
    return String(parseFloat(n.toPrecision(12)))
}

// null when it cannot be worked out (division by zero)
function compute(a, op, b) {
    var r
    switch (op) {
    case "+": r = a + b; break
    case "-": r = a - b; break
    case "*": r = a * b; break
    case "/": if (b === 0) return null; r = a / b; break
    default: r = b
    }
    return isFinite(r) ? r : null
}

function digits(s) {
    return s.replace(/[^0-9]/g, "").length
}

var symbols = { "+": "+", "-": "−", "*": "×", "/": "÷" }
