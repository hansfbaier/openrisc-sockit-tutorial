#!/bin/bash
PATH=$PATH:$(dirname $0)

# assert or1k reset
memtool -32 0xff322100=0x8

sleep 1

# load binary
memload $1

# reset wishbone (has aut100
memtool -32 0xff322100=0x9

sleep 1

# arm diila
#
# Set post trigger count (default = 32)
#memtool -32 0xff200004=0x3e0
memtool -32 0xff200004=0x20

#Set trigger skip count (default = 0)
memtool -32 0xff200008=0
memtool -32 0xff200000=$2

# deassert or1k reset
memtool -32 0xff322100=0x0
