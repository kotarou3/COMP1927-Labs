#!/usr/bin/nodejs
"use strict";

const isSea = {
    "Adriatic Sea": 1,
    "Atlantic Ocean": 1,
    "Bay of Biscay": 1,
    "Black Sea": 1,
    "English Channel": 1,
    "Ionian Sea": 1,
    "Irish Sea": 1,
    "Mediterranean Sea": 1,
    "North Sea": 1,
    "Tyrrhenian Sea": 1
};

let testCases = [
    // Provided tests
    ["London", "Manchester", ["rail", "road"]],
    ["London", "English Channel", ["boat"]],
    ["Le Havre", "English Channel", ["boat"]],
    ["Leipzig", "Hamburg", ["road"]],
    ["Paris", "Marseilles", ["rail"]],
    ["Santander", "Nantes", ["boat"]],
    ["Rome", "Naples", ["rail", "road", "boat"]],
    ["Black Sea", "Ionian Sea", ["boat"]],
    ["Munich", "Berlin", []],
    ["London", "Castle Dracula", []],

    // Some edge cases
    ["Mediterranean Sea", "Ionian Sea", ["boat"]],
    ["English Channel", "Nantes", []],
    ["London", "London", ["boat"]],
    ["Paris", "Paris", []]
];

let nodes = {};

for (let line of require("fs").readFileSync("euro.txt").toString().split("\n")) {
    let match = line.match(/^(.*) connects to (.*) by (road|rail|boat)$/);
    if (!match)
        continue;

    let from = match[1];
    let to = match[2];
    let by = match[3];

    if (!nodes[from])
        nodes[from] = {};
    if (!nodes[from][to])
        nodes[from][to] = [];

    nodes[from][to].push(by);
}

for (let start of Object.keys(nodes)) {
    for (let end of Object.keys(nodes)) {
        let connections = [];

        for (let adjacent of Object.keys(nodes[start])) {
            if (adjacent === end)
                Array.prototype.push.apply(connections, nodes[start][adjacent]);
            else if (isSea[adjacent] && nodes[adjacent][end])
                Array.prototype.push.apply(connections, nodes[adjacent][end]);
        }

        testCases.push([start, end, connections]);
    }
}

for (let test of testCases) {
    let from = test[0];
    let to = test[1];
    let connections = test[2];

    connections = connections.map(type => ({
        "road": "Road connection",
        "rail": "Rail connection",
        "boat": "Boat connection"
    }[type])).sort().join("\n");
    if (connections.length === 0)
        connections = "No direct connection";

    let output = require("child_process").spawnSync("./conn", [from, to]).stdout.toString();
    output = output.split("\n").slice(1).filter(line => !!line).sort().join("\n");
    if (output !== connections) {
        console.error(`${from} -> ${to} failed:`);
        console.error("Expected:");
        console.error(connections);
        console.error("Got:")
        console.error(output);
        process.exit(1);
    }
}

console.warn("Passed");
