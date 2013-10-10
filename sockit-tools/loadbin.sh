#!/bin/bash
PATH=$PATH:$(dirname $0)

# assert or1k reset
memtool -32 0xff322100=0x8

sleep 1

# load binary
memload $1

# reset wishbone (has auto deassert)
memtool -32 0xff322100=0x9

sleep 1

# deassert or1k reset
memtool -32 0xff322100=0x0
