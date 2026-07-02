#!/bin/bash
# use testnet settings,  if you need mainnet,  use ~/.filopowcore/filopowd.pid file instead
filopow_pid=$(<~/.filopowcore/testnet3/filopowd.pid)
sudo gdb -batch -ex "source debug.gdb" filopowd ${filopow_pid}
