#!/bin/sh

# 1--
for body in $(seq 120 60 240); do
    echo "Testing sequential N^2 n-body, args = $body 50000 1"
    echo "Testing sequential N^2 n-body, args = $body 50000 1" >> ./test.log
    for i in $(seq 1 5);do ./build/1-seqn2 $body 50000 1 >> ./test.log;done
    echo >> ./test.log
done

# 2--
for thread in 1 2 4; do
    for body in $(seq 120 60 240); do
        echo "Testing parallel N^2 n-body, args = $body $thread 50000 1"
        echo "Testing parallel N^2 n-body, args = $body $thread 50000 1" >> ./test.log
        for i in $(seq 1 5);do ./build/2-parn2 $body $thread 50000 1 >> ./test.log;done
        echo >> ./test.log
    done
done

# 3--
for body in $(seq 120 60 240); do
    echo "Testing Barnes-Hut n-body, args = $body 0.8 50000 1"
    echo "Testing Barnes-Hut n-body, args = $body 0.8 50000 1" >> ./test.log
    for i in $(seq 1 5);do ./build/3-seqBH $body 0.8 50000 1 >> ./test.log;done
    echo >> ./test.log
done

# 4--
for thread in 1 2 4; do
    for body in $(seq 120 60 240); do
        echo "Testing parallel Barnes-Hut n-body, args = $body $thread 0.8 50000 1"
        echo "Testing parallel Barnes-Hut n-body, args = $body $thread 0.8 50000 1" >> ./test.log
        for i in $(seq 1 5);do ./build/4-parBH $body $thread 0.8 50000 1 >> ./test.log;done
        echo >> ./test.log
    done
done
        