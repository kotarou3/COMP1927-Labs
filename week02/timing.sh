#!/bin/bash

if [ "$(uname)" = "Darwin" ]; then
    RANDOMISE="gsort -R"
else
    RANDOMISE="sort -R"
fi

function outputAvg {
    TYPE="$1"; shift
    IS_DUPE="$1"; shift
    INPUT="$1"; shift
    SAMPLESIZE="$1"; shift
    ITERATIONS=$((500000 / $SAMPLESIZE))

    USEL_TIME=$(avgTime "./usel" "$SAMPLESIZE" "$ITERATIONS" "$INPUT")
    SORT_TIME=$(avgTime "sort -n" "$SAMPLESIZE" "$ITERATIONS" "$INPUT")

    echo $SAMPLESIZE $'\t\t' $TYPE $'\t' $IS_DUPE $'\t\t' $ITERATIONS $'\t\t' $USEL_TIME $'\t\t' $SORT_TIME
}

function avgTime {
    COMMAND="$1"; shift
    SAMPLESIZE="$1"; shift
    ITERATIONS="$1"; shift
    INPUT="$1"; shift

    /usr/bin/time -p bash -c "$(cat <<EOF
for ((I=0; I<$ITERATIONS; ++I)); do
    head -n $SAMPLESIZE <($INPUT) | $COMMAND > /dev/null
done
EOF
    )" 2>&1 | grep real | awk '{printf ('$ITERATIONS' >= 20 ? "%.3f" : "%.2f"), $2 / '$ITERATIONS'}'
}

echo Input Size $'\t' Initial Order $'\t' Has Dupes $'\t' '# of Runs' $'\t' usel Time $'\t' sort Time

for j in 5000 10000 20000 50000 100000; do
    outputAvg sorted yes "./randl $j | sort -n" $j
    outputAvg sorted no "seq $j | sort -n" $j
    outputAvg reverse yes "./randl $j | sort -nr" $j
    outputAvg reverse no "seq $j | sort -nr" $j
    outputAvg random yes "./randl $j | $RANDOMISE" $j
    outputAvg random no "seq $j | $RANDOMISE" $j
done

cat <<EOF
Small Files - Negligible time, because they're small.
Bigger Files - Less Negligble time, because our algo sucks.
Large Files - Huge time (usel), because our algos suck. a lot.
Sort seems superior for all no trivial cases.
EOF
