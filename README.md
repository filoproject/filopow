FILOPOW Core
============

https://github.com/filoproject/filopow

To see how to run FILOPOW, please read the respective files in [the doc folder](doc).


What is FILOPOW?
----------------

FILOPOW is a GPU-mineable proof-of-work cryptocurrency with a **smartnode**
service tier. It is a fork of [Neoxa](https://github.com/NeoxaChain/Neoxa)
(itself Ravencoin + Dash's deterministic masternode stack), so it combines:

- **KawPow GPU mining** — the existing KawPow ecosystem (miners, pools, HiveOS)
  works with it directly;
- **Smartnodes** — nodes that lock collateral and provide InstantSend,
  **ChainLocks** (51%-attack protection), and network services for a share of
  every block reward;
- **Native assets** — anyone can issue and transfer tokens on-chain without
  writing a smart contract.

### Network at a glance

| Parameter | Value |
| --- | --- |
| Consensus | KawPow (Proof of Work) + ChainLocks |
| Ticker | FPOW |
| Target block time | 60 seconds |
| Base block reward | 5 FPOW |
| Halving interval | every 2,100,000 blocks (~4 years) |
| Maximum supply | ~21,000,000 FPOW |
| Reward split (first ~2 years) | 36% miner / 54% smartnode / 10% development |
| Reward split (after sunset) | 40% miner / 60% smartnode / 0% development |
| Smartnode collateral | 5,000 FPOW |
| Native asset protocol | yes (issue, transfer, reissue, restricted) |
| P2P / RPC port | 7767 / 7766 |
| P2PKH address prefix | `F` |

Smartnode payments activate automatically once **150 smartnodes** are
registered; until then their share goes to miners, so the earliest miners earn
the most. The smartnode share is deliberately the larger one: miners can sell
into day-one liquidity, while smartnode operators lock coins to earn — both
kinds of participants matter, and each is paid for what they provide.

### The development fee, in the open

Blocks 0 through 1,050,000 (~2 years) carry a **10% development fee**, paid
on-chain to a disclosed treasury address in every block, and **it then ends
permanently** (0% forever after — enforced by consensus, not policy). It exists
for one purpose: the operators do not premine and do not mine, so the fee is
the only inventory from which market liquidity (on filodex and elsewhere) can
be provided. Every coin of it is visible on-chain.

There is **no premine, no insider allocation, and no hidden mining**. The
launch time is announced in advance with code and binaries public before
genesis. See [doc/fair-launch.md](doc/fair-launch.md) for the commitments and
how to verify each one.


Running a Smartnode
-------------------

A smartnode locks **5,000 FPOW** as collateral and provides InstantSend,
ChainLocks, and chain services in exchange for its share of every block reward.
Setup follows the standard Dash-lineage `protx` registration flow; a step-by-step
guide lives in [doc/smartnodes.md](doc/smartnodes.md).


License
-------

FILOPOW Core is released under the terms of the MIT license. See
[COPYING](COPYING) for more information or see
https://opensource.org/licenses/MIT.

FILOPOW is a fork of Neoxa, which descends from Ravencoin, Dash, and Bitcoin.
The upstream developers retain their copyright over the code that predates this
fork, and we are grateful for all of it.


Development
-----------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/filoproject/filopow/tags) are
created regularly to indicate new official, stable release versions.

Build instructions live in the [`doc/`](doc/) directory:
[Unix](doc/build-unix.md), [macOS](doc/build-osx.md),
[Windows](doc/build-windows.md).
